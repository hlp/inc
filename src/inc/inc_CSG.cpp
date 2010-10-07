
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
#include <cinder/TriMesh.h>

#include <csg/Solid.h>

#include <inc/inc_CSG.h>

namespace inc {

std::tr1::shared_ptr<ci::TriMesh> CSG::csg_solid_to_tri_mesh(
    std::tr1::shared_ptr<csg::Solid> solid) {

    std::tr1::shared_ptr<csg::VectorSet> vertices = 
        std::tr1::shared_ptr<csg::VectorSet>(solid->getVertices());

    std::tr1::shared_ptr<csg::IntSet> indices = 
        std::tr1::shared_ptr<csg::IntSet>(solid->getIndices());


    std::tr1::shared_ptr<ci::TriMesh> mesh = 
        std::tr1::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    csg::mlVector3D v;
    for (int i = 0; i < vertices->GetSize(); ++i) {
        v = vertices->GetVector(i);

        mesh->appendVertex(ci::Vec3f(v.x, v.y, v.z));
    }

    for (int i = 0; i < indices->GetSize();) {
        mesh->appendTriangle(indices->GetInt(i),
            indices->GetInt(i+1), indices->GetInt(i+2));

        i += 3;
    }

    return mesh;
}

std::tr1::shared_ptr<csg::Solid> CSG::tri_mesh_to_csg_solid(
    std::tr1::shared_ptr<ci::TriMesh> mesh) {
    
    std::tr1::shared_ptr<csg::VectorSet> vertices = 
        std::tr1::shared_ptr<csg::VectorSet>(new csg::VectorSet());

    // colors must be equal to vertices
    std::tr1::shared_ptr<csg::ColorSet> colors = 
        std::tr1::shared_ptr<csg::ColorSet>(new csg::ColorSet());

    std::tr1::shared_ptr<csg::IntSet> indices = 
        std::tr1::shared_ptr<csg::IntSet>(new csg::IntSet());

    std::vector<ci::Vec3f> mesh_verts = mesh->getVertices();

    std::for_each(mesh_verts.begin(), mesh_verts.end(),
        [=] (const ci::Vec3f& v) {
            vertices->AddVector(csg::mlVector3D(v.x, v.y, v.z));
            colors->AddColor(csg::gxColor(0, 0, 0));
    } );

    std::vector<size_t> mesh_indices = mesh->getIndices();

    std::for_each(mesh_indices.begin(), mesh_indices.end(),
        [=] (size_t ind) {
            indices->AddInt(ind);
    } );

    std::tr1::shared_ptr<csg::Solid> solid = 
        std::tr1::shared_ptr<csg::Solid>(
        new csg::Solid(vertices.get(), indices.get(), colors.get()));

    return solid;
}

}