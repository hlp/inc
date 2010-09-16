
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

#include <cinder/TriMesh.h>
#include <cinder/Vector.h>

namespace inc {

class MeshCreator {
public:
    MeshCreator();

    std::tr1::shared_ptr<ci::TriMesh> generate_bag_mesh(ci::Vec3f center,
        float radius);

    // creates a mesh, turns that into a soft body, and adds it to the scene
    void add_solid_bag(ci::Vec3f center, float radius);

    static MeshCreator& instance();

private:
    static MeshCreator* instance_;

};

}