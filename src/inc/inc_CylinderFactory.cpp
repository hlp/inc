
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
#include <cinder/app/App.h>
#include <cinder/TriMesh.h>
#include <cinder/CinderMath.h>
#include <cinder/Rand.h>

#include <inc/inc_CylinderFactory.h>
#include <inc/inc_Solid.h>
#include <inc/inc_MeshCreator.h>
#include <inc/inc_Manager.h>
#include <incApp.h>

namespace inc {

CylinderFactory* CylinderFactory::instance_;

CylinderFactory::CylinderFactory() {
    instance_ = this;
}

CylinderFactory::~CylinderFactory() {
    // Nothing here
}

SoftSolidPtr CylinderFactory::create_soft_cylinder(std::pair<ci::Vec3f, 
    ci::Vec3f> centers, float radius, int slice_res, int rot_res) {
    // TODO: hook into slice and rotate res
    if (rot_res % 2 == 0) {
        ci::app::console() << "ERROR: cannot create cylinder with odd rotation resultion" << std::endl;
        return SoftSolidPtr(NULL);
    }

    TriMeshPtr mesh = MeshCreator::instance().generate_bspline_revolve_mesh(
        generate_cylinder_bspline(centers, radius), slice_res, rot_res);
    
    /*
    ci::Rand rand(IncApp::instance().getElapsedFrames());
    int rand_indice = rand.randFloat(mesh->getNumTriangles() - 5);
    
    std::tr1::shared_ptr<std::vector<int>> triangles(new std::vector<int>());

    for (int i = 0; i < 5; ++i) {
        triangles->push_back(rand_indice + i);
    }

    TriMeshPtr mesh_increase = MeshCreator::instance().increase_resolution(mesh,
        triangles, 4);

    SoftSolidPtr ptr = SolidFactory::create_soft_mesh(mesh_increase);
    */

    SoftSolidPtr ptr = SolidFactory::create_soft_mesh(mesh);

    Manager::instance().add_solid(ptr);

    return ptr;
}

SoftSolidPtr CylinderFactory::create_soft_cylinder_network(std::vector<
    std::pair<ci::Vec3f, ci::Vec3f>>, float radius, int resolution) {
    
    return SolidFactory::create_soft_sphere(ci::Vec3f(), ci::Vec3f::one());
}

std::tr1::shared_ptr<ci::BSpline3f> CylinderFactory::generate_cylinder_bspline(std::pair<
    ci::Vec3f, ci::Vec3f> centers, float radius) {
    std::vector<ci::Vec3f> points;

    // number of control points per disc
    int num_disc = 5;
    // number of control points for the side length
    int num_side = 10;

    ci::Vec3f start = centers.first;
    ci::Vec3f end = centers.second;
    ci::Vec3f axis = end - start;
    axis.normalize();
    ci::Vec3f alt = ci::Vec3f::yAxis();
    if (alt == axis)
        alt = ci::Vec3f::zAxis();
    // perp is the vector lying in the circles
    ci::Vec3f perp = axis.cross(alt);
    perp.normalize();

    // the current working point
    ci::Vec3f point = start;

    float disc_step = radius / (float) num_disc;
    float side_step = (end - start).length() / (float) num_side;

    // walk along the entire perimeter of the line, adding points
    // start with the center of the first disc
    points.push_back(point);

    // add points for the start disc
    for (int i = 0; i < num_disc; ++i) {
        point += perp * disc_step;
        points.push_back(point);
    }

    // add points for the side
    for (int i = 0; i < num_side; ++i) {
        point += axis * side_step;
        points.push_back(point);
    }

    // walk backwards on the top disc
    for (int i = 0; i < num_disc; ++i) {
        point -= perp * disc_step;
        points.push_back(point);
    }

    // 1st = points, 2nd = degree, 3rd = add points to close, 4th = is it open
    return std::tr1::shared_ptr<ci::BSpline3f>(new ci::BSpline3f(points, 3,
        false, true));
}

CylinderFactory& CylinderFactory::instance() {
    return *instance_;
}

}
