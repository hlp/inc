
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

#include <cinder/TriMesh.h>
#include <cinder/Vector.h>

namespace inc {

class Solid;
class MeshNetwork;

class MeshCreator {
public:
    MeshCreator();
    ~MeshCreator();

    void setup();

    std::tr1::shared_ptr<ci::TriMesh> generate_circle_mesh(ci::Vec3f center,
        float radius);
    std::tr1::shared_ptr<ci::TriMesh> generate_bspline_mesh(
        std::tr1::shared_ptr<ci::BSpline3f> mesh, float height);

    // creates a mesh, turns that into a soft body, and adds it to the scene
    void add_circle_mesh(ci::Vec3f center, float radius);
    void add_bspline_mesh(std::tr1::shared_ptr<ci::BSpline3f>);

    void add_obj_mesh(const std::string& file_name,
        ci::Vec3f scl = ci::Vec3f::one());
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