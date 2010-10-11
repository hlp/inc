
/*  Copyright (c) 2010, Patrick Tierney
 *
 *  This file is part of INC (INC's Not CAD).
 *
 *  INC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  INC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with INC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <BulletSoftBody/btSoftBody.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h>
#include <BulletDynamics/ConstraintSolver/btHingeConstraint.h>
#include <BulletDynamics/ConstraintSolver/btSliderConstraint.h>

#include <cinder/gl/gl.h>

#include <inc/inc_LinkFactory.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Manager.h>

namespace inc {

LinkFactory::LinkFactory() {
    instance_ = this;

    soft_cfm_ = 1.0f;
    soft_erp_ = 1.0f;

    tau_ = 0.3f;
	damping_ = 1.0f;
	impulse_clamp_ = 0.0f;

    sphere_radius_ = 1.0f;
    link_gap_ = 2.0f;
}

void LinkFactory::setup() {
    // nothing here
}

void LinkFactory::create_link_matrix(LinkType link_type, int w, int d,
    ci::Vec3f axis) {
    ci::Vec3f position = ci::Vec3f::zero();

    // create rigid bodies (spheres)
    std::vector<std::vector<btRigidBody*> > r_bodies;
    std::vector<std::vector<ci::Vec3f> > positions;

    r_bodies.resize(w);
    positions.resize(w);
    for (int i = 0; i < w; ++i) {
        r_bodies[i].resize(d);
        positions[i].resize(d);
    }

    ci::Vec3f ptemp = position;
    float r = sphere_radius_;
    float gap = link_gap_;
    ci::Vec3f xgap = ci::Vec3f(gap, 0.0f, 0.0f);
    ci::Vec3f zgap = ci::Vec3f(0.0f, 0.0f, gap);
    ci::Vec3f xdiam = ci::Vec3f(r*2.0f, 0.0f, 0.0f);
    ci::Vec3f zdiam = ci::Vec3f(0.0f, 0.0f, r*2.0f);
    int resolution = 20;

    std::tr1::shared_ptr<std::deque<SolidPtr> > d_ptr = 
        std::tr1::shared_ptr<std::deque<SolidPtr> >(new std::deque<SolidPtr>());
    
    for (int i = 0; i < w; ++i) {
            for (int k = 0; k < d; ++k) {
                ci::Vec3f p = position + xgap * i + zgap * k +
                    xdiam * i + zdiam * k;
                positions[i][k] = p;
                r_bodies[i][k] = 
                    SolidFactory::create_bullet_rigid_sphere(p, r);

                d_ptr->push_back(SolidPtr(new RigidSolid(
                    new SphereGraphicItem(r), r_bodies[i][k], 
                    SolidFactory::instance().dynamics_world())));
            }
    }

    // link rigid bodies together
    for (int i = 0; i < w; ++i) {
        for (int k = 0; k < d; ++k) {
            if (i > 0) {
                switch (link_type) {
                case HINGE:
                    hinge_link_rigid_bodies(
                        *(r_bodies[i-1][k]), *(r_bodies[i][k]),
                        positions[i-1][k], positions[i][k], axis);
                    break;
                case SOCKET:
                default:
                    socket_link_rigid_bodies(
                        *(r_bodies[i-1][k]), *(r_bodies[i][k]),
                        positions[i-1][k], positions[i][k]);
                    break;
                }
            }

            if (k > 0) {
                switch (link_type) {
                case HINGE:
                hinge_link_rigid_bodies(
                    *(r_bodies[i][k-1]), *(r_bodies[i][k]),
                    positions[i][k-1], positions[i][k], axis);
                break;
                case SOCKET:
                default:
                socket_link_rigid_bodies(
                    *(r_bodies[i][k-1]), *(r_bodies[i][k]),
                    positions[i][k-1], positions[i][k]);
                break;
                }
            }
        }
    }

    // add them to the scene
    std::for_each(d_ptr->begin(), d_ptr->end(),
        [] (SolidPtr s_ptr) {
            Manager::instance().solids().push_back(s_ptr);
    } );
}

void LinkFactory::socket_link_soft_bodies(btSoftBody& s1,
    btSoftBody& s2, const ci::Vec3f& p1, const ci::Vec3f& p2) {
    btSoftBody::LJoint::Specs lj;
    lj.cfm = soft_cfm_;
	lj.erp = soft_erp_;
    ci::Vec3f mid = (p1 + p2) / 2.0f;
	lj.position = btVector3(mid.x, mid.y, mid.z);
    s1.appendLinearJoint(lj, &s2);
}

void LinkFactory::socket_link_rigid_bodies(btRigidBody& r1,
    btRigidBody& r2, const ci::Vec3f& p1, const ci::Vec3f& p2) {

    btVector3 bt_p1(p1.x, p1.y, p1.z);
    btVector3 bt_p2(p2.x, p2.y, p2.z);

    btPoint2PointConstraint* socket =
        new btPoint2PointConstraint(r1, r2, bt_p1, bt_p2);

    socket->m_setting.m_damping = damping_;
    socket->m_setting.m_impulseClamp = impulse_clamp_;
    socket->m_setting.m_tau = tau_;
}

void LinkFactory::hinge_link_rigid_bodies(btRigidBody& r1, btRigidBody& r2,
    const ci::Vec3f& p1, const ci::Vec3f& p2, const ci::Vec3f& axis) {

    btVector3 bt_p1(p1.x, p1.y, p1.z);
    btVector3 bt_p2(p2.x, p2.y, p2.z);
    btVector3 bt_axis(axis.x, axis.y, axis.z);

    btHingeConstraint* hinge = 
        new btHingeConstraint(r1, r2, bt_p1, bt_p2, bt_axis, bt_axis);
}

LinkFactory* LinkFactory::instance_;

LinkFactory& LinkFactory::instance() {
    return *instance_;
}

}
