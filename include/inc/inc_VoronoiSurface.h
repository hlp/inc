
#pragma once

#include <vector>
#include <utility> // for pair

#include <cinder/Vector.h>

#include <inc/inc_Module.h>

namespace inc {

typedef std::pair<ci::Vec3f, ci::Vec3f> Line;
typedef std::vector<Line> LineVector;

class VoronoiSurface : public Module {
public:
    VoronoiSurface();

    void setup();
    void update();
    void draw();

private:
    void init();
    void load_random_points(int num_points);
    void calculate_voronoi_ownership();
    void create_lines_from_grid();
    
    void loft_lines(Line line1, Line line2);

    float max_post_height_;
    float post_unit_height_;
    float grid_unit_width_;
    float grid_unit_depth_;
    int grid_width_;
    int grid_depth_;
    int loft_res_; // number of lines to loft with

    std::vector<ci::Vec2i> starting_coords_;
    
    LineVector surface_lines_;
    // the grid of the surface is essntially divided up into pixels, and 
    // each starting point has overship over these pixels.
    // int represents the index in the starting_coords_ vector
    std::vector<std::vector<int>> point_ownership_;

    std::vector<std::vector<ci::Vec3f>> post_points_;
    std::vector<std::vector<bool>> grid_touched_;
};

}