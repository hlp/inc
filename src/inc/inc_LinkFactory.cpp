
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
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>

#include <cinder/gl/gl.h>
#include <cinder/app/App.h>

#include <inc/inc_LinkFactory.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Manager.h>
#include <inc/inc_LinkMesh.h>

namespace inc {

LinkFactory::LinkFactory() {
    instance_ = this;

    soft_cfm_ = 1.0f;
    soft_erp_ = 1.0f;

    tau_ = 0.3f;
	damping_ = 1.0f;
	impulse_clamp_ = 0.0f;

    sphere_radius_ = 1.0f;
    link_gap_ = sphere_radius_ * 2.0f;
}

void LinkFactory::setup() {
    // nothing here
}

void LinkFactory::create_link_matrix(LinkType link_type, int w, int d,
    ci::Vec3f axis) {
    ci::Vec3f position = ci::Vec3f(0.0f, sphere_radius_ * 30.0f, 0.0f);

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

    std::tr1::shared_ptr<std::deque<RigidSolidPtr> > d_ptr = 
        std::tr1::shared_ptr<std::deque<RigidSolidPtr> >(new std::deque<RigidSolidPtr>());
    
    for (int i = 0; i < w; ++i) {
            for (int k = 0; k < d; ++k) {
                ci::Vec3f p = position + xgap * i + zgap * k +
                    xdiam * i + zdiam * k;
                positions[i][k] = p;
                r_bodies[i][k] = 
                    SolidFactory::create_bullet_rigid_sphere(p, r);

                d_ptr->push_back(RigidSolidPtr(new RigidSolid(
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

std::shared_ptr<std::vector<JointPtr>> LinkFactory::link_rigid_body_matrix(
    int w, int d, LinkType link_type,
    std::shared_ptr<std::deque<RigidSolidPtr>> solids, ci::Vec3f axis,
    std::shared_ptr<std::vector<JointCellPtr>> joint_cells) {

    std::shared_ptr<std::vector<JointPtr>> joints = 
        std::shared_ptr<std::vector<JointPtr>>(
        new std::vector<JointPtr>());

    // link rigid bodies together
    for (int i = 0; i < w; ++i) {
        for (int k = 0; k < d; ++k) {
            if (i > 0) {
                switch (link_type) {
                case HINGE:
                    joints->push_back(HingeJointPtr(
                        new HingeJoint(hinge_link_rigid_bodies(
                            solids->at((i-1)*d + k)->rigid_body(),
                            solids->at(i*d + k)->rigid_body(),
                            solids->at((i-1)*d + k)->position(),
                            solids->at(i*d + k)->position(),
                            // replace axis with a vector
                            //((solids->at((i-1)*d + k)->position() + 
                            //solids->at(i*d + k)->position()) / 2.0f).normalized()
                            axis
                            ))));
                    break;
                case SOCKET:
                default:
                    joints->push_back(SocketJointPtr(
                        new SocketJoint(socket_link_rigid_bodies(
                            solids->at((i-1)*d + k)->rigid_body(),
                            solids->at(i*d + k)->rigid_body(),
                            solids->at((i-1)*d + k)->position(),
                            solids->at(i*d + k)->position()))));
                    break;
                }
            } // end if (i > 0)

            if (k > 0) {
                switch (link_type) {
                case HINGE:
                    joints->push_back(HingeJointPtr(
                        new HingeJoint(hinge_link_rigid_bodies(
                            solids->at(i*d + (k-1))->rigid_body(),
                            solids->at(i*d + k)->rigid_body(),
                            solids->at(i*d + (k-1))->position(),
                            solids->at(i*d + k)->position(), 
                            // replace axis with vector
                            //((solids->at(i*d + k)->position() + 
                            //solids->at(i*d + (k-1))->position()) / 2.0f).normalized()
                            axis
                            ))));
                break;
                case SOCKET:
                default:
                    joints->push_back(SocketJointPtr(
                        new SocketJoint(socket_link_rigid_bodies(
                            solids->at(i*d + (k-1))->rigid_body(),
                            solids->at(i*d + k)->rigid_body(),
                            solids->at(i*d + (k-1))->position(),
                            solids->at(i*d + k)->position()))));
                break;
                }
            } // end if (k > 0)

            /* i0 -- k0, k1, k2 ...
             * i1 -- k0, k1, k2 ...
             * i2 -- k0, k1, k2 ...
             * i3 -- k0, k1, k2 ...
             */
            std::vector<JointPtr> cell_vec;
            // make a joint cell, refer to LinkMesh.h for description
            if (i > 0 && k > 0) {
                // [0] 
                // if i == 1, a different number of joints were added to the first
                // row (one joint per solid) verses the second (usually two joints
                // per solid. Hence the need for this nasty if
                if (i > 1)
                    cell_vec.push_back(joints->at(joints->size() - 1 - (d*2 - 1)));
                else {
                    int offset_upper = k * 2 + 1;
                    int offset_lower = d - k - 1;
                    cell_vec.push_back(joints->at(joints->size() - 1
                        - (offset_upper + offset_lower)));
                }
                // [1] 
                cell_vec.push_back(joints->at(joints->size() - 2));
                // [2]
                cell_vec.push_back(joints->at(joints->size() - 1));
                // [3]
                if (k > 1)
                    cell_vec.push_back(joints->at(joints->size() - 4));
                else
                    cell_vec.push_back(joints->at(joints->size() - 3));

                joint_cells->push_back(std::shared_ptr<JointCell>(
                    new JointCell(cell_vec)));
                cell_vec.clear();
            }
        }
    }

    return joints;
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

btPoint2PointConstraint* LinkFactory::socket_link_rigid_bodies(btRigidBody& r1,
    btRigidBody& r2, const ci::Vec3f& p1, const ci::Vec3f& p2) {

    btVector3 bt_p1(p1.x, p1.y, p1.z);
    btVector3 bt_p2(p2.x, p2.y, p2.z);

    btVector3 mid = (bt_p2 - bt_p1) / 2.0;

    btPoint2PointConstraint* socket =
        new btPoint2PointConstraint(r1, r2, mid, -mid);
    
    socket->m_setting.m_damping = damping_;
    socket->m_setting.m_impulseClamp = impulse_clamp_;
    socket->m_setting.m_tau = tau_;

    SolidFactory::instance().dynamics_world()->addConstraint(socket);

    return socket;
}

btHingeConstraint* LinkFactory::hinge_link_rigid_bodies(btRigidBody& r1, 
    btRigidBody& r2, const ci::Vec3f& p1, const ci::Vec3f& p2, const ci::Vec3f& axis) {

    btVector3 bt_p1(p1.x, p1.y, p1.z);
    btVector3 bt_p2(p2.x, p2.y, p2.z);
    btVector3 bt_axis(axis.x, axis.y, axis.z);

    btVector3 mid = (bt_p2 - bt_p1) / 2.0;

    btHingeConstraint* hinge = 
        new btHingeConstraint(r1, r2, mid, -mid, bt_axis, bt_axis);
    
    SolidFactory::instance().dynamics_world()->addConstraint(hinge);

    return hinge;
}

LinkFactory* LinkFactory::instance_;

LinkFactory& LinkFactory::instance() {
    return *instance_;
}

}
