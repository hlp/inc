
#pragma once

#include <vector>
#include <utility> // for pair
#include <deque>

#include <cinder/app/AppBasic.h>
#include <cinder/Vector.h>
#include <cinder/Rand.h>

#include <inc/inc_Module.h>

namespace inc {

typedef std::pair<ci::Vec3f, ci::Vec3f> Line;
typedef std::vector<Line> LineVector;

class VoronoiSurface : public Module {
public:
    VoronoiSurface();
    ~VoronoiSurface();

    void setup();
    void update();
    void draw();

    virtual bool mouse_down(ci::app::MouseEvent);

private:
    void init();
    void load_random_points(int num_points);
    void load_random_colors();
    void calculate_voronoi_ownership();
    void calculate_max_dists();
    void create_lines_from_grid();
    void raise_post_points();
    void raise_post_points_lmap();

    void bredth_first_raise_point(int index, ci::Vec2i coord);
    void raise_point_recurse(int index, ci::Vec2i coord);
    
    void loft_lines(Line line1, Line line2);

    void reset();

    std::vector<float> last_height_;
    std::vector<ci::Vec2i> coord_queue_;

    float max_post_height_;
    float post_unit_height_;
    float grid_unit_width_;
    float grid_unit_depth_;
    int grid_width_;
    int grid_depth_;
    int loft_res_; // number of lines to loft with

    std::deque<ci::Vec2i> bfs_queue_;
    std::deque<ci::Vec2i>::iterator bfs_it;
    std::deque<ci::Vec2i> bfs_last_coord_;

    std::vector<ci::Vec2i> starting_coords_;
    std::vector<ci::Color> coord_colors_;

    std::vector<float> starting_coords_max_dists_;
    
    LineVector surface_lines_;
    std::vector<ci::Color> surface_colors_;
    // the grid of the surface is essntially divided up into pixels, and 
    // each starting point has overship over these pixels.
    // int represents the index in the starting_coords_ vector
    std::vector<std::vector<int>> point_ownership_;

    std::vector<std::vector<ci::Vec3f>> post_points_;
    std::vector<std::vector<bool>> grid_touched_;

    bool is_valid_coord(ci::Vec2i vec) {
        return vec.x >= 0 && vec.x < grid_width_ && vec.y >= 0 && vec.y < grid_depth_;
    }

    ci::Vec3f last_intersect_;

    ci::Rand rand;

    ci::CallbackId mouse_down_cb_id_;
};

}