
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

#pragma once

#include <vector>
#include <deque>

#include <cinder/Vector.h>

class btSoftBody;
class btRigidBody;
class btPoint2PointConstraint;
class btHingeConstraint;

namespace inc {

class Solid;
class RigidSolid;
class Joint;
typedef std::shared_ptr<Joint> JointPtr;
typedef std::shared_ptr<RigidSolid> RigidSolidPtr;
class JointCell;
typedef std::shared_ptr<JointCell> JointCellPtr;
class HingeJoint;
typedef std::shared_ptr<HingeJoint> HingeJointPtr;
class LinkMesh;

class LinkFactory {
public:
    LinkFactory();
    ~LinkFactory();
    void setup();

    enum LinkType {
        SOCKET = 0, // like an arm locket, 360 deg freedom
        HINGE = 1, // like a door hinge
        SLIDER = 2 // like a slide whistle
    };

    void create_link_matrix(LinkType, int num_x, int num_y, ci::Vec3f axis = ci::Vec3f::yAxis());

    void LinkFactory::socket_link_soft_bodies(btSoftBody& s1, btSoftBody& s2,
        const ci::Vec3f& p1, const ci::Vec3f& p2);
    btPoint2PointConstraint* LinkFactory::socket_link_rigid_bodies(btRigidBody& r1, 
        btRigidBody& r2, const ci::Vec3f& p1, const ci::Vec3f& p2);
    btHingeConstraint* LinkFactory::hinge_link_rigid_bodies(btRigidBody& r1, 
        btRigidBody& r2, const ci::Vec3f& p1, const ci::Vec3f& p2, const ci::Vec3f& axis);

    // returned vector order:
    // w0,d0  w0,d1  w0, d2 ... w1,d0  w1,d1  w1,d2 ...
    std::tr1::shared_ptr<std::vector<JointPtr>> link_rigid_body_matrix(
        int w, int d, LinkType link_type,
        std::tr1::shared_ptr<std::deque<RigidSolidPtr>> solids, 
        ci::Vec3f axis = ci::Vec3f::yAxis(),
        std::shared_ptr<std::vector<JointCellPtr>> joint_cells = 
        std::shared_ptr<std::vector<JointCellPtr>>(new std::vector<JointCellPtr>()));

    void add_hinge_motor(HingeJointPtr ptr, float target_velocity,
        float max_motor_impulse);
    void add_hinge_motor(HingeJointPtr ptr, float target_velocity,
        float max_motor_impulse, float motor_target, float target_scale);

    static LinkFactory& instance();

    float tau_;
    float damping_;
    float impulse_clamp_;
    float soft_cfm_;
    float soft_erp_;

    float sphere_radius_;
    float sphere_mass_;
    float link_gap_;

    int link_counter_;

    std::shared_ptr<LinkMesh> last_mesh_;

private:


    static LinkFactory* instance_;

    std::vector<std::tr1::shared_ptr<Solid> > current_matrix_;
};

}
