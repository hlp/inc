
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
#include <cinder/ObjLoader.h>

#include <csg/Solid.h>
#include <csg/BooleanModeller.h>

#include <inc/inc_MeshNetwork.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Manager.h>
#include <inc/inc_CSG.h>
#include <inc/inc_MeshCreator.h>

namespace inc {

MeshNetwork::MeshNetwork() {
    instance_ = this;
}

MeshNetwork::~MeshNetwork() {
    // nothing here
}

void MeshNetwork::create_tube_union() {
    ci::ObjLoader loader(ci::loadFileStream("data/tripod-2.obj"));
    ci::TriMesh tube_mesh;
    loader.load(&tube_mesh, true);

    std::tr1::shared_ptr<ci::TriMesh> tube_mesh_ptr = 
        std::tr1::shared_ptr<ci::TriMesh>(new ci::TriMesh(tube_mesh));

    // create two tubes (indice / vertice)
    std::tr1::shared_ptr<csg::Solid> tube_1 = 
        CSG::tri_mesh_to_csg_solid(tube_mesh_ptr);

    std::tr1::shared_ptr<csg::Solid> tube_2 = 
        CSG::tri_mesh_to_csg_solid(tube_mesh_ptr);

    tube_1->Scale(csg::mlVector3D(10.0f, 10.0f, 10.0f));
    tube_2->Scale(csg::mlVector3D(10.0f, 10.0f, 10.0f));

    tube_1->Translate(csg::mlVector3D(6.0f, 3.0f, 0.0f));

    csg::BooleanModeller bm(tube_1.get(), tube_2.get());

    std::tr1::shared_ptr<csg::Solid> tube_union = 
        std::tr1::shared_ptr<csg::Solid>(bm.getUnion());

    ci::Vec3f scl(1.0f, 1.0f, 1.0f);

    SolidPtr union_solid = SolidFactory::create_soft_mesh(
        CSG::csg_solid_to_tri_mesh(tube_union), scl);

    Manager::instance().add_solid(union_solid);
    MeshCreator::instance().set_current_mesh(union_solid);
}

MeshNetwork* MeshNetwork::instance_;

MeshNetwork& MeshNetwork::instance() {
    return *instance_;
}


}