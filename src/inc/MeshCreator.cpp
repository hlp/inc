
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
#include <cinder/CinderMath.h>
#include <cinder/app/App.h>

#include <inc/MeshCreator.h>
#include <inc/Manager.h>
#include <inc/Solid.h>

namespace inc {

MeshCreator::MeshCreator() {
    instance_ = this;
}

std::tr1::shared_ptr<ci::TriMesh> MeshCreator::generate_circle_mesh(
    ci::Vec3f center, float radius) {
    int line_res = 80; // 20 points per line
    int rot_res = 80; // rotate in 1/20th incrementns

    // make the mesh at the origin, 
    std::tr1::shared_ptr<std::vector<ci::Vec3f> > base_line = 
        make_half_circle(ci::Vec3f(0.0f, 0.0f, 0.0f), radius, line_res);

    // find axis, the line between the first and last points
    ci::Vec3f axis = (*base_line)[base_line->size() - 1] - (*base_line)[0u];

    std::vector<std::vector<ci::Vec3f> > line_segments;

    for (int i = 0; i < rot_res; ++i) {
        line_segments.push_back(std::vector<ci::Vec3f>());

        float theta = ci::lmap<float>(i, 0, rot_res - 1, M_PI, M_PI * 2.0f);
        
        for (int j = 0; j < line_res; ++j) {
            // rotate around the axis
            ci::Vec3f v = ci::Quatf(axis, theta) * (*base_line)[j];
            
            float stretch = 2.0f;
            v = ci::Vec3f(v.x, v.y * stretch, v.z);

            v += center;
            line_segments[i].push_back(v);
        }
    }

    std::tr1::shared_ptr<ci::TriMesh> mesh = 
        std::tr1::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    for (int i = 0; i < rot_res; ++i) {
        for (int j = 0; j < line_res; ++j) {
            mesh->appendVertex(line_segments[i][j]);
        }
    }

    auto append_by_index = [&mesh, &rot_res] (int i1, int j1,
        int i2, int j2, int i3, int j3) {
        int index1 = i1 * rot_res + j1;
        int index2 = i2 * rot_res + j2;
        int index3 = i3 * rot_res + j3;

        mesh->appendTriangle(index1, index2, index3);
    };

    for (int i = 1; i < rot_res; ++i) {
        for (int j = 1; j < line_res; ++j) {
            append_by_index(i-1, j-1, i-1, j, i, j);
            append_by_index(i-1, j-1, i, j-1, i, j);
        }
    }

    return mesh;
}

std::tr1::shared_ptr<std::vector<ci::Vec3f> > MeshCreator::make_half_circle(
    ci::Vec3f center, float radius, int res) {

    std::tr1::shared_ptr<std::vector<ci::Vec3f> > curve = 
        std::tr1::shared_ptr<std::vector<ci::Vec3f> >(new std::vector<ci::Vec3f>());

    ci::Vec3f new_point;

    for(int i = 0; i < res; ++i) {
        float theta = ci::lmap<float>(i, 0, res - 1, 0, M_PI);

        new_point = ci::Vec3f(ci::math<float>::cos(theta) * radius, 0.0f,
            ci::math<float>::sin(theta) * radius);

        new_point += center;

        curve->push_back(new_point);
    }

    return curve;
}


void MeshCreator::add_circle_mesh(ci::Vec3f center, float radius) {
    std::tr1::shared_ptr<ci::TriMesh> mesh = generate_circle_mesh(center, radius);

    Manager::instance().add_solid(SolidFactory::create_soft_mesh(mesh));
}

void MeshCreator::add_bspline_mesh(std::tr1::shared_ptr<ci::BSpline3f> bspline) {
    std::tr1::shared_ptr<ci::TriMesh> mesh = generate_bspline_mesh(bspline);

    Manager::instance().add_solid(SolidFactory::create_soft_mesh(mesh));
}

std::tr1::shared_ptr<ci::TriMesh> MeshCreator::generate_bspline_mesh(
    std::tr1::shared_ptr<ci::BSpline3f> bspline) {
    int rot_res = 10;
    int slice_res = 10;
    int num_slices = (slice_res - 2) / 2 + 2;

    std::vector<ci::Vec3f> points;

    ci::Vec3f start_point;
    ci::Vec3f mid_point;
    std::vector<ci::Vec3f> base_points;

    for (int i = 0; i < slice_res; ++i) {
        float t = ci::lmap<float>(i, 0, slice_res - 1, 0, 1.0f);

        ci::Vec3f v = bspline->getPosition(t);

        if (i == 0) {
            start_point = v;
            continue;
        }

        if (i == slice_res / 2) {
            mid_point = v;
            continue;
        }

        base_points.push_back(v);
    }

    std::tr1::shared_ptr<std::vector<ci::Vec3f> > start_arc = 
        make_vertical_arc(start_point, start_point, rot_res);

    std::for_each(start_arc->begin(), start_arc->end(),
        [&] (ci::Vec3f vec) { points.push_back(vec); } );

    int j = base_points.size() - 1;
    for (int i = 0; i < base_points.size() / 2; ++i) {
        std::tr1::shared_ptr<std::vector<ci::Vec3f> > temp_arc = 
            make_vertical_arc(base_points[i], base_points[j],
            rot_res);

        std::for_each(temp_arc->begin(), temp_arc->end(),
            [&] (ci::Vec3f vec) { points.push_back(vec); } );

        --j;
    }

    std::tr1::shared_ptr<std::vector<ci::Vec3f> > end_arc = 
        make_vertical_arc(mid_point, mid_point, rot_res);

    std::for_each(end_arc->begin(), end_arc->end(),
        [&] (ci::Vec3f vec) { points.push_back(vec); } );

    // stitch all the points together into a mesh

    std::tr1::shared_ptr<ci::TriMesh> mesh = 
        std::tr1::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    std::for_each(points.begin(), points.end(), 
        [&] (ci::Vec3f vec) { mesh->appendVertex(vec); } );

    auto append_by_index = [&mesh, &rot_res] (int i1, int j1,
        int i2, int j2, int i3, int j3) {
        int index1 = i1 * rot_res + j1;
        int index2 = i2 * rot_res + j2;
        int index3 = i3 * rot_res + j3;

        mesh->appendTriangle(index1, index2, index3);
    };

    for (int i = 1; i < num_slices; ++i) {
        for (int j = 1; j < rot_res; ++j) {
            append_by_index(i-1, j-1, i-1, j, i, j);
            append_by_index(i-1, j-1, i, j-1, i, j);
        }
    }

    return mesh;
}

std::tr1::shared_ptr<std::vector<ci::Vec3f> > MeshCreator::make_vertical_arc(
    const ci::Vec3f& p1, const ci::Vec3f& p2, int segments) {
    ci::Vec3f center = (p1 + p2) / 2.0f;

    std::tr1::shared_ptr<std::vector<ci::Vec3f> > points = 
        std::tr1::shared_ptr<std::vector<ci::Vec3f> >(
        new std::vector<ci::Vec3f>());

    ci::Vec3f axis = p2 - p1;

    ci::Vec3f start_point = p1 - center; 

    for (int i = 0; i < segments; ++i) {
        float theta = ci::lmap<float>(i, 0, segments - 1, M_PI, M_PI * 2.0f);

        ci::Vec3f v = ci::Quatf(axis, theta) * start_point;
        float stretch = 2.0f;
        v = ci::Vec3f(v.x, v.y * stretch, v.z);

        v += center;

        points->push_back(v);
    }

    return points;
}


MeshCreator* MeshCreator::instance_;

MeshCreator& MeshCreator::instance() {
    return *instance_;
}


}