
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

#pragma once

#include <utility> // for pair
#include <vector>

#include <cinder/Vector.h>

namespace inc {

class SoftSolid;
typedef std::tr1::shared_ptr<SoftSolid> SoftSolidPtr;

class CylinderFactory {
public:
    CylinderFactory();
    virtual ~CylinderFactory();

    static CylinderFactory& instance();

    SoftSolidPtr create_soft_cylinder(std::pair<ci::Vec3f, ci::Vec3f>,
        float radius, int resolution);

    SoftSolidPtr create_soft_cylinder_network(std::vector<std::pair<ci::Vec3f,
        ci::Vec3f>>, float radius, int resolution);

private:
    std::tr1::shared_ptr<ci::BSpline3f> generate_cylinder_bspline(std::pair<
        ci::Vec3f, ci::Vec3f>, float radius);

    static CylinderFactory* instance_;

};

}