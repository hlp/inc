
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
#include <csg/BooleanModeller.h>

#include <inc/inc_MeshNetwork.h>
#include <inc/Solid.h>
#include <inc/Manager.h>

namespace inc {

MeshNetwork::MeshNetwork() {
    instance_ = this;
}

MeshNetwork::~MeshNetwork() {
    // nothing here
}

void MeshNetwork::create_tube_union() {
    // create two tubes (indice / vertice)
    csg::gxColor color(100, 100, 100);
    csg::Solid tube_1("data/csg/cylinder.txt", color, 100, 100, 100);
    csg::Solid tube_2("data/csg/cylinder.txt", color, 100, 100, 100);

    tube_1.Scale(csg::mlVector3D(10.0f, 10.0f, 10.0f));
    tube_2.Scale(csg::mlVector3D(10.0f, 10.0f, 10.0f));

    tube_1.Translate(csg::mlVector3D(6.0f, 3.0f, 0.0f));

    csg::BooleanModeller bm(&tube_1, &tube_2);

    csg::Solid* tube_union = bm.getUnion();

    csg::VectorSet* vertices = tube_union->getVertices();
    csg::IntSet* indices = tube_union->getIndices();

    //csg::VectorSet* vertices = tube_2.getVertices();
    //csg::IntSet* indices = tube_2.getIndices();

    ci::TriMesh mesh;

    for (int i = 0; i < vertices->GetSize(); ++i) {
        csg::mlVector3D v = vertices->GetVector(i);

        mesh.appendVertex(ci::Vec3f(v.x, v.y, v.z));
    }

    for (int i = 0; i < indices->GetSize();) {
        mesh.appendTriangle(indices->GetInt(i),
            indices->GetInt(i+1), indices->GetInt(i+2));

        i += 3;
    }

    std::tr1::shared_ptr<ci::TriMesh> mesh_ptr = 
        std::tr1::shared_ptr<ci::TriMesh>(new ci::TriMesh(mesh));

    ci::Vec3f scl(1.0f, 1.0f, 1.0f);

    SolidPtr union_solid = SolidFactory::create_soft_mesh(mesh_ptr, scl);

    Manager::instance().add_solid(union_solid);

    delete indices;
    delete vertices;
    delete tube_union;
}

MeshNetwork* MeshNetwork::instance_;

MeshNetwork& MeshNetwork::instance() {
    return *instance_;
}


}