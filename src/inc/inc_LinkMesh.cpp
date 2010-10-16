
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

#include <cinder/gl/gl.h>

#include <inc/inc_LinkMesh.h>
#include <inc/inc_SolidCreator.h>
#include <inc/inc_Manager.h>
#include <inc/inc_LinkFactory.h>

namespace inc {

LinkMesh::LinkMesh(int w, int d, LinkFactory::LinkType type,
    std::tr1::shared_ptr<std::deque<RigidSolidPtr>> solids) :
    w_(w), d_(d) {

    // copy the deque into a more efficient vector
    std::for_each(solids->begin(), solids->end(), [&] (RigidSolidPtr ptr) {
        solids_.push_back(ptr);
    } );

    // link all the solids together based on the link type
    LinkFactory::instance().link_rigid_body_matrix(w, d, type, solids, ci::Vec3f::yAxis());
}

LinkMesh::~LinkMesh() {
    solids_.clear();
}

std::tr1::shared_ptr<LinkMesh> LinkMesh::create_link_mesh(int w, int d,
    float sphere_radius, float spacing_scale, LinkFactory::LinkType type) {

    float height = 5.0f;
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

void LinkMesh::draw() {
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
}

}