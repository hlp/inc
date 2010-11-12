
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

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h>
#include <BulletDynamics/ConstraintSolver/btHingeConstraint.h>
#include <BulletDynamics/ConstraintSolver/btSliderConstraint.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>

#include <cinder/gl/gl.h>
#include <cinder/Rand.h>
#include <cinder/TriMesh.h>

#include <inc/inc_LinkMesh.h>
#include <inc/inc_SolidCreator.h>
#include <inc/inc_Manager.h>
#include <inc/inc_LinkFactory.h>
#include <incApp.h>
#include <inc/inc_DxfSaver.h>

namespace inc {

int LinkMesh::new_mesh_w_;
int LinkMesh::new_mesh_d_;
float LinkMesh::new_mesh_height_;
int LinkMesh::num_lock_points_;
ci::Vec3f LinkMesh::hinge_axis_;
float LinkMesh::line_weight_;

LinkMesh::LinkMesh(int w, int d, LinkFactory::LinkType type,
    std::tr1::shared_ptr<std::deque<RigidSolidPtr>> solids) :
    w_(w), d_(d) {

    // copy the deque into a more efficient vector
    std::for_each(solids->begin(), solids->end(), [&] (RigidSolidPtr ptr) {
        solids_.push_back(ptr);
    } );

    // create joint cells from joint array
    joint_cells_ = std::shared_ptr<std::vector<JointCellPtr>>(
        new std::vector<JointCellPtr>());
        
    // link all the solids together based on the link type
    joints_ = LinkFactory::instance().link_rigid_body_matrix(w, d, 
        type, solids, hinge_axis_, joint_cells_);

    // lock random points 
    ci::Rand rand;
    rand.seed(IncApp::instance().getElapsedFrames());
    int range = w_ * d_;
    for (int i = 0; i < num_lock_points_; ++i) {
        solids_[rand.nextInt(range)]->rigid_body().setMassProps(
            0.0f, btVector3(0.0, 0.0, 0.0));
    }
}

LinkMesh::~LinkMesh() {
    btDynamicsWorld* dw = SolidFactory::instance().dynamics_world();

    std::for_each(joints_->begin(), joints_->end(), [=] (JointPtr ptr) {
        dw->removeConstraint(ptr->constraint_ptr());
    } );
    
    joints_.reset();
    solids_.clear();
}

std::tr1::shared_ptr<LinkMesh> LinkMesh::create_link_mesh(int w, int d,
    float sphere_radius, float spacing_scale, LinkFactory::LinkType type) {

    ci::Vec3f offset = SolidCreator::instance().creation_point();
    
    float height = new_mesh_height_;
    // create solids
    std::tr1::shared_ptr<std::deque<RigidSolidPtr>> solids =
        std::tr1::shared_ptr<std::deque<RigidSolidPtr>>(
        new std::deque<RigidSolidPtr>());

    float axis_dist = (sphere_radius * 2 + sphere_radius*spacing_scale);

    ci::Vec3f r_pos;

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < d; ++j) {
            r_pos = offset + ci::Vec3f(i * axis_dist, 0.0f, j * axis_dist);
            r_pos.y = height;

            RigidSolidPtr s = SolidCreator::instance().create_rigid_sphere(r_pos,
                ci::Vec3f::one() * sphere_radius);

            // make sure that the solids aren't being drawn
            s->set_visible(false);

            solids->push_back(s);
        }
    }

    // create new link mesh using solids

    std::tr1::shared_ptr<LinkMesh> mesh_ptr = 
        std::tr1::shared_ptr<LinkMesh>(new LinkMesh(w, d, type, solids));

    Manager::instance().add_graphic_item(mesh_ptr);

    return mesh_ptr;
}

void LinkMesh::draw() {
    std::for_each(joint_cells_->begin(), joint_cells_->end(),
        [] (JointCellPtr ptr) {
            ptr->calculate_triangles();
            ptr->draw();
    });

    glLineWidth(line_weight_);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);

    std::for_each(joints_->begin(), joints_->end(),
        [] (JointPtr ptr) {
            ci::Vec3f a_pos = ptr->a_position();
            ci::Vec3f b_pos = ptr->b_position();
            ci::Vec3f pos = ptr->position();

            glVertex3f(pos);
            glVertex3f(a_pos);

            glVertex3f(pos);
            glVertex3f(b_pos);
    } );

    glEnd();
}

void LinkMesh::save(Exporter& exporter) {
    std::for_each(joint_cells_->begin(), joint_cells_->end(),
        [&] (JointCellPtr ptr) {
            ptr->save(exporter);
    });

    exporter.add_layer();

    std::for_each(joints_->begin(), joints_->end(),
        [&] (JointPtr ptr) {
            ci::Vec3f a_pos = ptr->a_position();
            ci::Vec3f b_pos = ptr->b_position();
            ci::Vec3f pos = ptr->position();

            exporter.write_line(pos, a_pos);
            exporter.write_line(pos, b_pos);
    } );
}

ci::Vec3f HingeJoint::a_position() {
    btVector3 vec = hinge_->getRigidBodyA().getWorldTransform().getOrigin();

    return ci::Vec3f(vec.x(), vec.y(), vec.z());
}

ci::Vec3f HingeJoint::b_position() {
    btVector3 vec = hinge_->getRigidBodyB().getWorldTransform().getOrigin();

    return ci::Vec3f(vec.x(), vec.y(), vec.z());
}

// NOTE: btDiscreteDynamicsWorld.cpp debugDrawConstraint has code that extracts
// position information from joints
ci::Vec3f HingeJoint::position() {
    btTransform tr = hinge_->getRigidBodyA().getCenterOfMassTransform() * hinge_->getAFrame();
    btVector3 pos = tr.getOrigin();
    
    return ci::Vec3f(pos.x(), pos.y(), pos.z());
}



ci::Vec3f SocketJoint::a_position() {
    btVector3 vec = socket_->getRigidBodyA().getWorldTransform().getOrigin();

    return ci::Vec3f(vec.x(), vec.y(), vec.z());
}

ci::Vec3f SocketJoint::b_position() {
    btVector3 vec = socket_->getRigidBodyB().getWorldTransform().getOrigin();

    return ci::Vec3f(vec.x(), vec.y(), vec.z());
}

ci::Vec3f SocketJoint::position() {
    btVector3 pivot_in_a = socket_->getPivotInA();
	pivot_in_a = socket_->getRigidBodyA().getCenterOfMassTransform() * pivot_in_a; 

    return ci::Vec3f(pivot_in_a.x(), pivot_in_a.y(), pivot_in_a.z());
}


void JointCell::calculate_triangles() {
    ci::Vec3f solid_1 = joints_[0]->a_position();
    ci::Vec3f solid_2 = joints_[0]->b_position();
    ci::Vec3f solid_3 = joints_[2]->b_position();
    ci::Vec3f solid_4 = joints_[2]->a_position();

    ci::Vec3f joint_0 = joints_[0]->position();
    ci::Vec3f joint_1 = joints_[1]->position();
    ci::Vec3f joint_2 = joints_[2]->position();
    ci::Vec3f joint_3 = joints_[3]->position();
        
    // calculate the exterior triangles

    normals_[0] = (joint_0 - solid_1).cross(joint_3 - solid_1).normalized();
    triangles_[0][0] = solid_1;
    triangles_[0][1] = joint_0;
    triangles_[0][2] = joint_3;

    normals_[1] = (joint_1 - solid_2).cross(joint_0 - solid_2).normalized();
    triangles_[1][0] = solid_2;
    triangles_[1][1] = joint_0;
    triangles_[1][2] = joint_1;

    normals_[2] = (joint_2 - solid_3).cross(joint_1 - solid_3).normalized();
    triangles_[2][0] = solid_3;
    triangles_[2][1] = joint_1;
    triangles_[2][2] = joint_2;

    normals_[3] = (joint_3 - solid_4).cross(joint_2 - solid_4).normalized();
    triangles_[3][0] = solid_4;
    triangles_[3][1] = joint_2;
    triangles_[3][2] = joint_3;

    // calculate the interior triangles

    normals_[4] = (joint_0 - joint_3).cross(joint_2 - joint_3).normalized();
    triangles_[4][0] = joint_0;
    triangles_[4][1] = joint_2;
    triangles_[4][2] = joint_3;

    normals_[5] = (joint_2 - joint_1).cross(joint_0 - joint_1).normalized();
    triangles_[5][0] = joint_0;
    triangles_[5][1] = joint_1;
    triangles_[5][2] = joint_2;
}

void JointCell::draw() {
    Color::set_color_a(color_);

    glBegin(GL_TRIANGLES);

    std::vector<ci::Vec3f>::const_iterator norm_it = normals_.begin();
    for (std::vector<std::vector<ci::Vec3f>>::const_iterator it = triangles_.begin();
        it != triangles_.end(); ++it) {
        glNormal3f(*norm_it);
        glVertex3f(it->at(0));
        glVertex3f(it->at(1));
        glVertex3f(it->at(2));
        ++norm_it;
    }

    glEnd();
}

void JointCell::save(Exporter& exporter) {
    ci::Vec3f solid_1 = joints_[0]->a_position();
    ci::Vec3f solid_2 = joints_[0]->b_position();
    ci::Vec3f solid_3 = joints_[2]->b_position();
    ci::Vec3f solid_4 = joints_[2]->a_position();
        
    exporter.write_triangle(solid_1, joints_[0]->position(),
        joints_[3]->position());

    exporter.write_triangle(solid_2, joints_[0]->position(),
        joints_[1]->position());

    exporter.write_triangle(solid_3, joints_[1]->position(),
        joints_[2]->position());
        
    exporter.write_triangle(solid_4, joints_[2]->position(),
        joints_[3]->position());
        
    // draw the interior triangles
    exporter.write_triangle(joints_[0]->position(), joints_[2]->position(),
        joints_[3]->position());

    exporter.write_triangle(joints_[0]->position(), joints_[1]->position(),
        joints_[2]->position());
}


}