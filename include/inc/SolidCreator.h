
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

/* SolidCreator:
   - creates basisc shapes or structures, and adds them to the scene.
*/

#pragma once

#include <boost/cstdint.hpp>

#include <cinder/params/Params.h>

#include <inc/Module.h>

namespace inc {

class SolidCreator : public Module {
public:
    SolidCreator();
    virtual ~SolidCreator();

    void setup();
    void update();
    void draw();

    void create_rigid_sphere(ci::Vec3f pos, ci::Vec3f radius);
    void create_soft_sphere(ci::Vec3f pos, ci::Vec3f radius);
    void create_linked_spheres(ci::Vec3f pos, ci::Vec3f radius);
    void create_sphere_matrix(ci::Vec3f pos, ci::Vec3f radius,
        int w, int h, int d);
    void create_sphere_spring_matrix(ci::Vec3f pos, ci::Vec3f radius,
        int w, int h, int d);
    void create_rigid_sphere_matrix(ci::Vec3f pos, ci::Vec3f radius,
        int w, int h, int d);

    void create_solid_box(ci::Vec3f pos, ci::Vec3f size);
    void load_obj_as_rigid(ci::Vec3f pos, ci::Vec3f scale);

    static SolidCreator& instance();

private:
    static SolidCreator* instance_;

};

}