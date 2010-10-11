
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

#include <inc/inc_LinkFactory.h>

namespace inc {

LinkFactory::LinkFactory() {
    instance_ = this;

    soft_cfm_ = 1.0f;
    soft_erp_ = 1.0f;

    tau_ = 0.3f;
	damping_ = 1.0f;
	impulse_clamp_ = 0.0f;
}

void LinkFactory::setup() {
    // nothing here
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

    btPoint2PointConstraint socket(r1, r2, bt_p1, bt_p2);

    socket.m_setting.m_damping = damping_;
    socket.m_setting.m_impulseClamp = impulse_clamp_;
    socket.m_setting.m_tau = tau_;
}

void LinkFactory::hinge_link_rigid_bodies(btRigidBody& r1, btRigidBody& r2,
    const ci::Vec3f& p1, const ci::Vec3f& p2, const ci::Vec3f& axis) {

    btVector3 bt_p1(p1.x, p1.y, p1.z);
    btVector3 bt_p2(p2.x, p2.y, p2.z);
    btVector3 bt_axis(axis.x, axis.y, axis.z);

    btHingeConstraint hinge(r1, r2, bt_p1, bt_p2, bt_axis, bt_axis);
}



void LinkFactory::create_socket_matrix(int mat_x, int mat_y) {
    // create rigid bodies (spheres)

    // link rigid bodies together

    // add them to the scene
}

LinkFactory* LinkFactory::instance_;

LinkFactory& LinkFactory::instance() {
    return *instance_;
}

}
