
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

#include <utility>

#include <cinder/TriMesh.h>
#include <cinder/Vector.h>

namespace inc {

class Solid;
class MeshNetwork;

typedef std::tr1::shared_ptr<ci::TriMesh> TriMeshPtr;

class MeshCreator {
public:
    MeshCreator();
    ~MeshCreator();

    void setup();

    // this creates a circular dome mesh
    TriMeshPtr generate_circle_mesh(ci::Vec3f center,
        float radius);
    // this takes a closed bspline mesh and makes it into a dome
    TriMeshPtr generate_bspline_dome_mesh(
        std::shared_ptr<ci::BSpline3f> bspline, float height);
    // this takes an open bspline mesh and revolves it (using the Ray between
    // the first and last points as the axis)
    // slice_res = the number of points to sample in the bspline
    // rot_res = the number of times to rotate
    TriMeshPtr generate_bspline_revolve_mesh(
        std::tr1::shared_ptr<ci::BSpline3f> bspline, int slice_res, int rot_res);
    // similar to above but only returns the points
    std::tr1::shared_ptr<std::vector<ci::Vec3f>> generate_bspline_revolve_points(
        std::shared_ptr<ci::BSpline3f> bspline, int slice_res, int rot_res);

    TriMeshPtr increase_resolution(TriMeshPtr, 
        std::shared_ptr<std::vector<int>> triangle_indices, int amount);

    std::shared_ptr<std::vector<ci::Vec3<ci::Vec3f>>> split_triangle(
        ci::Vec3<ci::Vec3f>, int amount);

    // creates a mesh, turns that into a soft body, and adds it to the scene
    void add_circle_mesh(ci::Vec3f center, float radius);
    void add_bspline_mesh(std::shared_ptr<ci::BSpline3f>);

    // loads an obj and creates a soft body mesh from it, and adds it 
    // to the scene
    void add_obj_mesh(const std::string& file_name,
        ci::Vec3f scl = ci::Vec3f::one());

    // these are utility methods that should be refactored
    void add_tripod_mesh();
    void add_anemone_mesh();

    // the mesh menu hooks into these
    bool adjust_mesh_scale(float);
    float* mesh_scale_ptr();

    bool adjust_arch_resolution(int);
    int* arch_resolution_ptr();
    bool adjust_slice_resolution(int);
    int* slice_resolution_ptr();

    int* tripod_legs_ptr() { return &tripod_legs_; }
    int* anemone_legs_ptr() { return &anemone_legs_; }
    bool adjust_tripod_mesh_scale(float);
    float* tripod_mesh_scale_ptr() { return &tripod_mesh_scale_; }
    bool adjust_anemone_mesh_scale(float);
    float* anemone_mesh_scale_ptr() { return &anemone_mesh_scale_; }

    static MeshCreator& instance();
    static MeshCreator* instance_ptr(); // used for the menu to hook into

    void draw();

    bool is_pointed_up();

    void rebuild_mesh();

    std::tr1::shared_ptr<MeshNetwork> mesh_network();

    // this assumes that the object has been added to the manager
    void set_current_mesh(std::tr1::shared_ptr<Solid> mesh);

private:
    std::vector<ci::Vec3<int>> patch_cirle(const std::vector<int>& indices);
    std::tr1::shared_ptr<std::vector<ci::Vec3f> > make_half_circle(
        ci::Vec3f center, float radius, int res);
    // join two points with a vertical arc
    std::tr1::shared_ptr<std::vector<ci::Vec3f> > make_vertical_arc(
        const ci::Vec3f& p1, const ci::Vec3f& p2, int segments, float stretch);

    std::tr1::shared_ptr<Solid> current_mesh_;

    std::tr1::shared_ptr<ci::TriMesh> debug_mesh_;

    std::tr1::shared_ptr<std::vector<ci::Vec3f> > arc_debug_;

    std::tr1::shared_ptr<MeshNetwork> mesh_network_;

    float mesh_scale_;
    int arch_resolution_;
    int slice_resolution_;
    bool is_pointed_up_; // a dome vs a basket

    int tripod_legs_;
    int anemone_legs_;
    float tripod_mesh_scale_;
    float anemone_mesh_scale_;

    static MeshCreator* instance_;
};

}