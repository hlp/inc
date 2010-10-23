
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

#include <inc/inc_CylinderFactory.h>
#include <inc/inc_Solid.h>

namespace inc {

CylinderFactory* CylinderFactory::instance_;

CylinderFactory::CylinderFactory() {
    instance_ = this;
}

CylinderFactory::~CylinderFactory() {
    // Nothing here
}

SoftSolidPtr CylinderFactory::create_soft_cylinder(std::pair<ci::Vec3f, 
    ci::Vec3f>, float radius, int resolution) {
    

    return SolidFactory::create_soft_sphere(ci::Vec3f(), ci::Vec3f::one());
}

SoftSolidPtr CylinderFactory::create_soft_cylinder_network(std::vector<
    std::pair<ci::Vec3f, ci::Vec3f>>, float radius, int resolution) {
    
    return SolidFactory::create_soft_sphere(ci::Vec3f(), ci::Vec3f::one());
}

std::tr1::shared_ptr<ci::BSpline3f> CylinderFactory::generate_cylinder_bspline(std::pair<
    ci::Vec3f, ci::Vec3f>, float radius) {

    return std::tr1::shared_ptr<ci::BSpline3f>(NULL);
}

CylinderFactory& CylinderFactory::instance() {
    return *instance_;
}

}
