
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

namespace inc {

LinkMesh::LinkMesh(int w, int d, LinkFactory::LinkType,
    std::tr1::shared_ptr<std::deque<SolidPtr>> solids) {

}

std::tr1::shared_ptr<LinkMesh> LinkMesh::create_link_mesh(int w, int d,
    float sphere_radius, float spacing_scale, LinkFactory::LinkType type) {

    float height = 5.0f;
    // create solids
    std::tr1::shared_ptr<std::deque<SolidPtr>> solids;

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < d; ++j) {
            float axis_dist = (sphere_radius + sphere_radius*spacing_scale);
            SolidPtr s = SolidCreator::instance().create_rigid_sphere(
                ci::Vec3f(w * axis_dist, height, j * axis_dist),
                ci::Vec3f::one() * sphere_radius);

            solids->push_back(s);
        }
    }

    // create new link mesh using solids
}

void LinkMesh::draw() {
    // make sure that the solids aren't being drawn
    // draw a TriMesh equivalent
}




}