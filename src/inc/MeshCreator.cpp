
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
#include <cinder/ObjLoader.h>

#include <inc/MeshCreator.h>
#include <inc/Manager.h>
#include <inc/Solid.h>
#include <inc/CurveSketcher.h>
#include <inc/inc_MeshNetwork.h>

namespace inc {

MeshCreator::MeshCreator() {
    instance_ = this;
    mesh_scale_ = 1.0f;
    is_pointed_up_ = true;
    arch_resolution_ = 50;
    slice_resolution_ = 50;

    tripod_legs_ = 3;
    anemone_legs_ = 3;
    tripod_mesh_scale_ = 15.0f;
    anemone_mesh_scale_ = 10.0f;

    current_mesh_ = std::tr1::shared_ptr<Solid>();
}

MeshCreator::~MeshCreator() {
    mesh_network_.reset();
}

void MeshCreator::setup() {
    mesh_network_ = std::tr1::shared_ptr<MeshNetwork>(new MeshNetwork());
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

void MeshCreator::draw() {
    if (debug_mesh_.get() == NULL)
        return;

    glLineWidth(1.0f);

    glBegin(GL_LINES);

    ci::Vec3f v1;
    ci::Vec3f v2;
    ci::Vec3f v3;

    for (int i = 0; i < debug_mesh_->getNumTriangles(); ++i) {
        debug_mesh_->getTriangleVertices(i, &v1, &v2, &v3);

        ci::gl::vertex(v1);
        ci::gl::vertex(v2);

        ci::gl::vertex(v2);
        ci::gl::vertex(v3);

        ci::gl::vertex(v1);
        ci::gl::vertex(v3);
    }

    glEnd();
}

// there's a problem with this method.
void MeshCreator::add_bspline_mesh(std::tr1::shared_ptr<ci::BSpline3f> bspline) {
    std::tr1::shared_ptr<ci::TriMesh> mesh = generate_bspline_mesh(bspline, 
        mesh_scale_);

    current_mesh_ = SolidFactory::create_soft_mesh(mesh);

    Manager::instance().add_solid(current_mesh_);
}

std::tr1::shared_ptr<ci::TriMesh> MeshCreator::generate_bspline_mesh(
    std::tr1::shared_ptr<ci::BSpline3f> bspline, float height) {
    int rot_res = arch_resolution_;
    int slice_res = slice_resolution_; // the number of points to sample the bspline
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

    for (int i = 0; i < rot_res; ++i) {
        points.push_back(start_point);
    }

    int j = base_points.size() - 1;
    for (int i = 0; i < base_points.size() / 2; ++i) {
        std::tr1::shared_ptr<std::vector<ci::Vec3f> > temp_arc = 
            make_vertical_arc(base_points[i], base_points[j],
            rot_res, height);

        std::for_each(temp_arc->begin(), temp_arc->end(),
            [&] (ci::Vec3f vec) { points.push_back(vec); } );

        --j;
    }

    for (int i = 0; i < rot_res; ++i) {
        points.push_back(mid_point);
    }

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
    const ci::Vec3f& p1, const ci::Vec3f& p2, int segments, float stretch) {
    ci::Vec3f center = (p1 + p2) / 2.0f;

    std::tr1::shared_ptr<std::vector<ci::Vec3f> > points = 
        std::tr1::shared_ptr<std::vector<ci::Vec3f> >(
        new std::vector<ci::Vec3f>());

    ci::Vec3f axis = p2 - p1;
    axis = axis.cross(ci::Vec3f::yAxis());

    ci::Vec3f start_point = p1 - center; 

    for (int i = 0; i < segments; ++i) {
        float theta;

        if (is_pointed_up_)
            theta = ci::lmap<float>(i, 0, segments - 1, M_PI, M_PI * 2.0f);
        else
            theta = ci::lmap<float>(i, 0, segments - 1, 0, M_PI);

        ci::Vec3f v = ci::Quatf(axis, theta) * start_point;
        // adjust the height depending on user params
        v = ci::Vec3f(v.x, v.y * stretch, v.z);

        v += center;

        points->push_back(v);
    }

    return points;
}

bool MeshCreator::adjust_mesh_scale(float scale) {
    mesh_scale_ = scale;

    rebuild_mesh();

    return false;
}

void MeshCreator::rebuild_mesh() {
    if (CurveSketcher::instance().invalid_curve())
        return;

    // delete old mesh, if it exists, and add a new mesh
    Manager::instance().remove_solid(current_mesh_);
    // this should really be rethought ...
    add_bspline_mesh(CurveSketcher::instance().current_spline());
}

void MeshCreator::set_current_mesh(std::tr1::shared_ptr<Solid> mesh) {
    Manager::instance().remove_solid(current_mesh_);

    current_mesh_ = mesh;
}

float* MeshCreator::mesh_scale_ptr() {
    return &mesh_scale_;
}

bool MeshCreator::adjust_arch_resolution(int res) {
    arch_resolution_ = res;

    rebuild_mesh();

    return false;
}

int* MeshCreator::arch_resolution_ptr() {
    return &arch_resolution_;
}

bool MeshCreator::adjust_slice_resolution(int res) {
    slice_resolution_ = res;

    rebuild_mesh();

    return false;
}

int* MeshCreator::slice_resolution_ptr() {
    return &slice_resolution_;
}

bool MeshCreator::is_pointed_up() {
    return is_pointed_up_;
}

bool MeshCreator::adjust_tripod_mesh_scale(float scl) {
    tripod_mesh_scale_ = scl;

    add_tripod_mesh();

    return false;
}

bool MeshCreator::adjust_anemone_mesh_scale(float scl) {
    anemone_mesh_scale_ = scl;

    add_anemone_mesh();

    return false;
}

void MeshCreator::add_obj_mesh(const std::string& file_name,
    ci::Vec3f scl) {
    Manager::instance().remove_solid(current_mesh_);

    ci::ObjLoader loader(ci::loadFileStream(file_name));
    ci::TriMesh mesh;
    loader.load(&mesh, true);

    std::tr1::shared_ptr<ci::TriMesh> mesh_ptr = 
        std::tr1::shared_ptr<ci::TriMesh>(new ci::TriMesh(mesh));

    current_mesh_ = SolidFactory::create_soft_mesh(mesh_ptr, scl);

    Manager::instance().add_solid(current_mesh_);
}

void MeshCreator::add_tripod_mesh() {
    std::string file_name;

    switch (tripod_legs_) {
    case 1:
        file_name = "tripod-1.obj";
        break;
    case 2:
        file_name = "tripod-2.obj";
        break;
    case 3:
        file_name = "tripod-3.obj";
        break;
    case 4:
        file_name = "tripod-4.obj";
        break;
    case 5:
    default:
        file_name = "tripod-5.obj";
        break;
    }

    file_name = "data/" + file_name;

    add_obj_mesh(file_name, ci::Vec3f::one() * tripod_mesh_scale_);
}

void MeshCreator::add_anemone_mesh() {
    std::string file_name;

    switch (anemone_legs_) {
    case 1:
        file_name = "anemone-1.obj";
        break;
    case 2:
        file_name = "anemone-2.obj";
        break;
    case 3:
        file_name = "anemone-3.obj";
        break;
    case 4:
        file_name = "anemone-4.obj";
        break;
    case 5:
    default:
        file_name = "anemone-5.obj";
        break;
    }

    file_name = "data/" + file_name;

    add_obj_mesh(file_name, ci::Vec3f::one() * anemone_mesh_scale_);
}

std::tr1::shared_ptr<MeshNetwork> MeshCreator::mesh_network() {
    return mesh_network_;
}

MeshCreator* MeshCreator::instance_ = NULL;

MeshCreator& MeshCreator::instance() {
    return *instance_;
}

MeshCreator* MeshCreator::instance_ptr() {
    return instance_;
}

}