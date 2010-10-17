
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

#include <inc/inc_LinkMesh.h>
#include <inc/inc_SolidCreator.h>
#include <inc/inc_Manager.h>
#include <inc/inc_LinkFactory.h>
#include <incApp.h>

namespace inc {

int LinkMesh::new_mesh_w_;
int LinkMesh::new_mesh_d_;
float LinkMesh::new_mesh_height_;
int LinkMesh::num_lock_points_;
ci::Vec3f LinkMesh::hinge_axis_;

LinkMesh::LinkMesh(int w, int d, LinkFactory::LinkType type,
    std::tr1::shared_ptr<std::deque<RigidSolidPtr>> solids) :
    w_(w), d_(d) {

    // copy the deque into a more efficient vector
    std::for_each(solids->begin(), solids->end(), [&] (RigidSolidPtr ptr) {
        solids_.push_back(ptr);
    } );

    // link all the solids together based on the link type
    joints_ = LinkFactory::instance().link_rigid_body_matrix(w, d, 
        type, solids, hinge_axis_);

    // lock 5 random points 
    ci::Rand rand;
    rand.seed(IncApp::instance().getElapsedFrames());
    int range = w_ * d_;
    for (int i = 0; i < num_lock_points_; ++i) {
        solids_[rand.nextInt(range)]->rigid_body().setMassProps(
            0.0f, btVector3(0.0, 0.0, 0.0));
    }
}

LinkMesh::~LinkMesh() {
    solids_.clear();
}

std::tr1::shared_ptr<LinkMesh> LinkMesh::create_link_mesh(int w, int d,
    float sphere_radius, float spacing_scale, LinkFactory::LinkType type) {

    float height = new_mesh_height_;
    // create solids
    std::tr1::shared_ptr<std::deque<RigidSolidPtr>> solids =
        std::tr1::shared_ptr<std::deque<RigidSolidPtr>>(
        new std::deque<RigidSolidPtr>());

    float axis_dist = (sphere_radius * 2 + sphere_radius*spacing_scale);

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < d; ++j) {
            
            RigidSolidPtr s = SolidCreator::instance().create_rigid_sphere(
                ci::Vec3f(i * axis_dist, height, j * axis_dist),
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

void LinkMesh::draw_joint(JointPtr joint) {

}

void LinkMesh::draw() {

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

    /*
    // Hinge line drawing code

    glBegin(GL_LINES);

    for (int i = 0; i < w_-1; ++i) {
        for (int j = 0; j < d_-1; ++j) {
            if (i == 0) {
                if (j != 0) {
                    glVertex3f(joints_->at(j-1)->position());
                    glVertex3f(joints_->at(j)->position());
                }
            } else if (j == 0) { // && i != 0
                glVertex3f(joints_->at((i-1)*w_ + j)->position());
                glVertex3f(joints_->at(i*w_ + j)->position());
            } else {
                glVertex3f(joints_->at((i-1)*w_ + j)->position());
                glVertex3f(joints_->at(i*w_ + j)->position());

                glVertex3f(joints_->at(i*w_ + (j-1))->position());
                glVertex3f(joints_->at(i*w_ + j)->position());
            }
        }
    }

    glEnd();
    */

    /*
    glBegin(GL_LINES);

    for (int i = 0; i < w_; ++i) {
        for (int j = 0; j < d_; ++j) {
            if (i == 0) {
                if (j != 0) {
                    glVertex3f(solids_[j-1]->position());
                    glVertex3f(solids_[j]->position());
                }
            } else if (j == 0) { // && i != 0
                glVertex3f(solids_[(i-1)*w_ + j]->position());
                glVertex3f(solids_[i*w_ + j]->position());
            } else {
                glVertex3f(solids_[(i-1)*w_ + j]->position());
                glVertex3f(solids_[i*w_ + j]->position());

                glVertex3f(solids_[i*w_ + (j-1)]->position());
                glVertex3f(solids_[i*w_ + j]->position());
            }
        }
    }

    glEnd();
    */

    /*
    ci::gl::enableWireframe();

    for (int i = 1; i < w_; ++i) {
        glBegin(GL_TRIANGLE_STRIP);

        for (int j = 0; j < d_; ++j) {
            glVertex3f(solids_[(i-1)*w_ + j]->position());
            glVertex3f(solids_[i*w_ + j]->position());
        }

        glEnd();
    }

    ci::gl::disableWireframe();
    */
}

ci::Vec3f HingeJoint::a_position() {
    btVector3 vec = hinge_->getRigidBodyA().getWorldTransform().getOrigin();

    return ci::Vec3f(vec.x(), vec.y(), vec.z());
}

ci::Vec3f HingeJoint::b_position() {
    btVector3 vec = hinge_->getRigidBodyB().getWorldTransform().getOrigin();

    return ci::Vec3f(vec.x(), vec.y(), vec.z());
}

ci::Vec3f HingeJoint::position() {
    btVector3 a_pos = hinge_->getRigidBodyA().getWorldTransform().getOrigin();
    
    btTransform trans = hinge_->getAFrame();
    btVector3 vec = trans.getOrigin();
    
    return ci::Vec3f(a_pos.x() + vec.x(), a_pos.y() + vec.y(), 
        a_pos.z() + vec.z());
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
    btVector3 a_pos = socket_->getRigidBodyA().getWorldTransform().getOrigin();
    btVector3 pivot_in_a = socket_->getPivotInA();

    return ci::Vec3f(a_pos.x() + pivot_in_a.x(),
        a_pos.y() + pivot_in_a.y(), a_pos.z() + pivot_in_a.z());
}

}