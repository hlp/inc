
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

#include <inc/MeshCreator.h>

namespace inc {

MeshCreator::MeshCreator() {
    instance_ = this;
}

std::tr1::shared_ptr<ci::TriMesh> MeshCreator::generate_bag(
    ci::Vec3f center, float radius) {
    std::tr1::shared_ptr<ci::TriMesh> mesh = 
        std::tr1::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    return mesh;
}


MeshCreator* MeshCreator::instance_;

MeshCreator& MeshCreator::instance() {
    return *instance_;
}


}