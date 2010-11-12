
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

#include <memory>
#include <stdexcept>
#include <sstream>

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h>
#include <BulletDynamics/ConstraintSolver/btHingeConstraint.h>
#include <BulletDynamics/ConstraintSolver/btSliderConstraint.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>

#include <cinder/gl/gl.h>
#include <cinder/Rand.h>
#include <cinder/TriMesh.h>
#include <cinder/ImageIo.h>

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
ci::Vec3f LinkMesh::hinge_axis_;
float LinkMesh::line_weight_;

LinkMesh::LinkMesh(int w, int d, LinkFactory::LinkType type,
    std::tr1::shared_ptr<std::deque<RigidSolidPtr>> solids) :
    w_(w), d_(d) {

    // copy the deque into a more efficient vector
    std::for_each(solids->begin(), solids->end(), [&] (RigidSolidPtr ptr) {
        solids_.push_back(ptr);
    } );
        
    // link all the solids together based on the link type
    joints_ = LinkFactory::instance().link_rigid_body_matrix(w, d, 
        type, solids, joint_cells_, hinge_axis_);

    isolate_hinges();
}

LinkMesh::LinkMesh(int w, int d, std::shared_ptr<std::deque<RigidSolidPtr>> 
    solids, std::vector<std::vector<ci::Vec3f>> axis_w,
    std::vector<std::vector<ci::Vec3f>> axis_d) : w_(w), d_(d) {

    // copy the deque into a more efficient vector
    std::for_each(solids->begin(), solids->end(), [&] (RigidSolidPtr ptr) {
        solids_.push_back(ptr);
    } );
        
    // hinge link all the solids together based on the axes
    joints_ = LinkFactory::instance().link_rigid_body_matrix(w, d, 
        solids, joint_cells_, axis_w, axis_d);

    isolate_hinges();
}

// isolate out all the hinges
void LinkMesh::isolate_hinges() {
    hinge_joints_ = std::shared_ptr<std::vector<HingeJointPtr>>(
        new std::vector<HingeJointPtr>());

    std::for_each(joints_->begin(), joints_->end(),
        [=] (JointPtr ptr) {
            if (dynamic_cast<HingeJoint*>(ptr.get()) != NULL)
                hinge_joints_->push_back(std::dynamic_pointer_cast<HingeJoint>(ptr));
    } );
}

LinkMesh::~LinkMesh() {
    btDynamicsWorld* dw = SolidFactory::instance().dynamics_world();

    std::for_each(joints_->begin(), joints_->end(), [=] (JointPtr ptr) {
        dw->removeConstraint(ptr->constraint_ptr());
    } );
    
    joints_.reset();
    solids_.clear();
}

std::shared_ptr<LinkMesh> LinkMesh::create_link_mesh(int w, int d,
    float sphere_radius, float spacing_scale, LinkFactory::LinkType type) {
    ci::Vec3f offset = SolidCreator::instance().creation_point();
    // create solids
    std::shared_ptr<std::deque<RigidSolidPtr>> solids = create_mesh_solids(
        w, d, sphere_radius, spacing_scale, 
        ci::Vec3f(offset.x, new_mesh_height_, offset.z));

    // create new link mesh using solids

    std::tr1::shared_ptr<LinkMesh> mesh_ptr = 
        std::tr1::shared_ptr<LinkMesh>(new LinkMesh(w, d, type, solids));

    Manager::instance().add_graphic_item(mesh_ptr);

    LinkFactory::instance().last_mesh_ = mesh_ptr;

    return mesh_ptr;
}

std::shared_ptr<LinkMesh> LinkMesh::create_from_images(const std::string& file_1,
    const std::string& file_2, float sphere_radius, float spacing_scale) throw(std::runtime_error) {

    ci::Surface axes_w = ci::loadImage(file_1); // has full width
    ci::Surface axes_d = ci::loadImage(file_2); // has full depth

    if (axes_w.getWidth() != axes_d.getWidth() + 1 || 
        axes_w.getHeight() + 1 != axes_d.getHeight())
        throw(std::runtime_error("incorrect initial image sizes"));

    int w = axes_w.getWidth();
    int d = axes_d.getHeight();

    ci::Surface::Iter it = axes_w.getIter();

    std::vector<std::vector<ci::Vec3f>> values_w;

    int i;

    for (i = 0; i < w; ++i) {
        values_w.push_back(std::vector<ci::Vec3f>());
        values_w[i].resize(d-1);
    }

    i = 0;
    while(it.line()) {
        int j = 0;
		while(it.pixel()) {
            ci::Vec3f v = ci::Vec3f(it.r() / 255.0f, it.g() / 255.0f, it.b() / 255.0f);
            values_w[j][i] = v;
            j++;
		}
        ++i;
	}

    if (values_w.size() != w || values_w[0].size() != d - 1) {
        std::stringstream ss;
        ss << "Unable to parse 1st image\n";
        ss << "Expected width = " << w << "\n";
        ss << "Actual width = " << values_w.size() << "\n";
        ss << "Expected depth = " << d - 1 << "\n";
        ss << "Actual depth = " << values_w[0].size() << "\n";
        throw(std::runtime_error(ss.str()));
    }

    std::vector<std::vector<ci::Vec3f>> values_d;

    for (i = 0; i < w-1; ++i) {
        values_d.push_back(std::vector<ci::Vec3f>());
        values_d[i].resize(d);
    }

    it = axes_d.getIter();

    i = 0;
    while(it.line()) {
        int j = 0;
		while(it.pixel()) {
            ci::Vec3f v = ci::Vec3f(it.r() / 255.0f, it.g() / 255.0f, it.b() / 255.0f);
            values_d[j][i] = v;
            j++;
		}
        ++i;
	}

    if (values_d.size() != w - 1 || values_d[0].size() != d) {
        std::stringstream ss;
        ss << "Unable to parse 2nd image\n";
        ss << "Expected width = " << w - 1 << "\n";
        ss << "Actual width = " << values_d.size() << "\n";
        ss << "Expected depth = " << d << "\n";
        ss << "Actual depth = " << values_d[0].size() << "\n";
        throw(std::runtime_error(ss.str()));
    }

    ci::Vec3f offset = SolidCreator::instance().creation_point();

    std::shared_ptr<std::deque<RigidSolidPtr>> solids = create_mesh_solids(
        w, d, sphere_radius, spacing_scale, 
        ci::Vec3f(offset.x, new_mesh_height_, offset.z));

    // create new link mesh using solids

    std::tr1::shared_ptr<LinkMesh> mesh_ptr = 
        std::tr1::shared_ptr<LinkMesh>(new LinkMesh(w, d, solids,
        values_w, values_d));

    Manager::instance().add_graphic_item(mesh_ptr);

    LinkFactory::instance().last_mesh_ = mesh_ptr;

    return mesh_ptr;
}

std::shared_ptr<std::deque<RigidSolidPtr>> LinkMesh::create_mesh_solids(int w, int d, 
    float sphere_rad, float spacing, ci::Vec3f origin) {
    std::shared_ptr<std::deque<RigidSolidPtr>> solids =
        std::shared_ptr<std::deque<RigidSolidPtr>>(new std::deque<RigidSolidPtr>());

    float axis_dist = (sphere_rad * 2 + sphere_rad*spacing);

    ci::Vec3f r_pos = origin;

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < d; ++j) {
            r_pos = origin + ci::Vec3f(i * axis_dist, 0.0f, j * axis_dist);

            RigidSolidPtr s = SolidCreator::instance().create_rigid_sphere(r_pos,
                ci::Vec3f::one() * sphere_rad);
            
            // todo check that this works
            btVector3 inertia(0,0,0);
	        float mass = LinkFactory::instance().sphere_mass_;
            s->rigid_body_ptr()->getCollisionShape()->calculateLocalInertia(mass, 
                inertia);
            s->rigid_body_ptr()->setMassProps(LinkFactory::instance().sphere_mass_,
                inertia);

            // make sure that the solids aren't being drawn
            s->set_visible(false);

            solids->push_back(s);
        }
    }

    return solids;
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

void LinkMesh::activate() {
    std::for_each(solids_.begin(), solids_.end(), 
        [] (RigidSolidPtr ptr) {
            ptr->rigid_body_ptr()->activate();
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