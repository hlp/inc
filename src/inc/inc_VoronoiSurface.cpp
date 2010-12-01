
#include <utility>
#include <algorithm>

#include <boost/date_time.hpp>

#include <cinder/gl/gl.h>
#include <cinder/Rand.h>

#include <incApp.h>
#include <inc/inc_VoronoiSurface.h>
#include <inc/inc_CurveSketcher.h>
#include <inc/inc_Camera.h>

namespace inc {

VoronoiSurface::VoronoiSurface() {
    max_post_height_ = 6.0f;
    post_unit_height_ = 1.0f;
    grid_width_ = 40;
    grid_depth_ = 100;
    grid_unit_width_ = 4.0f;
    grid_unit_depth_ = 4.0f;
    loft_res_ = 6; 

    last_intersect_ = ci::Vec3f(100, 100, 100);
}

VoronoiSurface::~VoronoiSurface() {
    IncApp::instance().unregisterMouseDown(mouse_down_cb_id_);
}

void VoronoiSurface::setup() {
    mouse_down_cb_id_ = IncApp::instance().registerMouseDown(this, 
        &VoronoiSurface::mouse_down);

    init();

    load_random_points(1);
    load_random_colors();

    // calculate voronoi from points to know which point owns each post on the surface
    calculate_voronoi_ownership();
    
    calculate_max_dists();

    // march along lines grid, outward from points, raising posts
    raise_post_points();

    // stitch the post points into lines
    create_lines_from_grid();
}

void VoronoiSurface::init() {
    // set up point_ownership_ vector
    point_ownership_.clear();
    
    for (int i = 0; i < grid_width_; ++i) {
        point_ownership_.push_back(std::vector<int>());
        point_ownership_[i].resize(grid_depth_);
    }

    // set up post_points_ vector
    post_points_.clear();
    for (int i = 0; i < grid_width_; ++i) {
        post_points_.push_back(std::vector<ci::Vec3f>());
        for (int j = 0; j < grid_depth_; ++j) {
            post_points_[i].push_back(ci::Vec3f(i * grid_unit_width_,
                0.0f, j * grid_unit_depth_));
        }
    }

    // set up the grid_touched_ vector
    grid_touched_.clear();
    for (int i = 0; i < grid_width_; ++i) {
        grid_touched_.push_back(std::vector<bool>());
        for (int j = 0; j < grid_depth_; ++j) {
            grid_touched_[i].push_back(false);
        }
    }
}

void VoronoiSurface::load_random_points(int num_points) {
    rand.randomize();

    for (int i = 0; i < num_points; ++i) {
        starting_coords_.push_back(ci::Vec2i(rand.randInt(grid_width_),
            rand.randInt(grid_depth_)));
    }
}

void VoronoiSurface::load_random_colors() {
    coord_colors_.clear();
    for (int i = 0; i < starting_coords_.size(); ++i) {
        coord_colors_.push_back(ci::Color(rand.randFloat(1.0f),
            rand.randFloat(1.0f), rand.randFloat(1.0f)));
    }
}


void VoronoiSurface::calculate_voronoi_ownership() {
    for (int xx = 0; xx < grid_width_; xx++) {
        for (int yy=0; yy < grid_depth_; yy++) {
            int idx = 0;
            float dd = starting_coords_[0].distance(ci::Vec2f(xx, yy));
            for (int i = 1; i < starting_coords_.size(); i++ ) {
                float d0 = starting_coords_[i].distance(ci::Vec2f(xx,yy));
                if (dd > d0) {
                    dd = d0;
                    idx = i;
                }
            }
            point_ownership_[xx][yy] = idx;
        }
    }
}

void VoronoiSurface::calculate_max_dists() {
    starting_coords_max_dists_.resize(starting_coords_.size());

    float max;
    for (int i = 0; i < starting_coords_.size(); ++i) {
        max = 0.0f;
        for (int w = 0; w < grid_width_; ++w) {
            for (int d = 0; d < grid_depth_; ++d) {
                if (point_ownership_[w][d] != i)
                    continue;

                float dist = ci::Vec2f(w * grid_unit_width_, 
                    d * grid_unit_depth_).distance(ci::Vec2f(
                    starting_coords_[i].x * grid_unit_width_, 
                    starting_coords_[i].y * grid_unit_depth_));

                if (dist > max)
                    max = dist;
            }
        }

        starting_coords_max_dists_[i] = max;
    }
}


void VoronoiSurface::create_lines_from_grid() {
    for (int i = 1; i < grid_width_; ++i) {
        for (int j = 0; j < grid_depth_; ++j) {
            surface_colors_.push_back(coord_colors_[point_ownership_[i][j]]);

            surface_lines_.push_back(Line(post_points_[i-1][j],
                post_points_[i][j]));
        }
    }

    // These are redundant when drawing lofts
    /*
    for (int i = 0; i < grid_width_; ++i) {
        for (int j = 1; j < grid_depth_; ++j) {
            surface_lines_.push_back(Line(post_points_[i][j-1],
                post_points_[i][j]));
        }
    }
    */

    for (int i = 1; i < grid_width_; ++i) {
        for (int j = 1; j < grid_depth_; ++j) {

            for (int k = 0; k < loft_res_; ++k) {
                surface_colors_.push_back(coord_colors_[point_ownership_[i][j]]);
            }

            loft_lines(Line(post_points_[i-1][j-1], post_points_[i][j-1]),
                Line(post_points_[i-1][j], post_points_[i][j]));
        }
    }
}

void VoronoiSurface::loft_lines(Line line1, Line line2) {
    ci::Vec3f line_1_start = line1.first;
    ci::Vec3f line_1_end = line1.second;

    ci::Vec3f line_2_start = line2.first;
    ci::Vec3f line_2_end = line2.second;

    ci::Vec3f loft_start, loft_end;

    for (int i = 0; i < loft_res_; ++i) {
        loft_start = ci::Vec3f(
            ci::lmap<float>(i, 0, loft_res_, line_1_start.x, line_1_end.x),
            ci::lmap<float>(i, 0, loft_res_, line_1_start.y, line_1_end.y),
            ci::lmap<float>(i, 0, loft_res_, line_1_start.z, line_1_end.z));
        loft_end = ci::Vec3f(
            ci::lmap<float>(i, 0, loft_res_, line_2_start.x, line_2_end.x),
            ci::lmap<float>(i, 0, loft_res_, line_2_start.y, line_2_end.y),
            ci::lmap<float>(i, 0, loft_res_, line_2_start.z, line_2_end.z));

        surface_lines_.push_back(Line(loft_start, loft_end));
    }
}

void VoronoiSurface::raise_post_points() {
    raise_post_points_lmap();

    /*
    for (int i = 0; i < starting_coords_.size(); ++i) {
        bredth_first_raise_point(i, starting_coords_[i]);
    }
    */
}

void VoronoiSurface::raise_post_points_lmap() {
    for (int i = 0; i < starting_coords_.size(); ++i) {

        for (int w = 0; w < grid_width_; ++w) {
            for (int d = 0; d < grid_depth_; ++d) {
                if (point_ownership_[w][d] != i)
                    continue;

                float dist = ci::Vec2f(w * grid_unit_width_, 
                    d * grid_unit_depth_).distance(
                    ci::Vec2f(starting_coords_[i].x * grid_unit_width_, 
                    starting_coords_[i].y * grid_unit_depth_));

                int m_dist = ci::math<int>::abs(starting_coords_[i].x - w) + 
                    ci::math<int>::abs(starting_coords_[i].y - d);

                /*
                post_points_[w][d].y = ci::math<float>::clamp(
                    ci::math<float>::floor(m_dist / 2 * post_unit_height_), 
                    0.0f, max_post_height_);
                */
                
                post_points_[w][d].y = ci::math<float>::floor(ci::lmap<float>(
                    dist, 0, starting_coords_max_dists_[i], 0, max_post_height_));
                
                /*
                post_points_[w][d].y = ci::math<float>::clamp(
                    ci::math<float>::floor(dist / grid_unit_width_), 0.0f,
                    max_post_height_);
                */
            }
        }

    }
}

void VoronoiSurface::bredth_first_raise_point(int index, ci::Vec2i coord) {
    grid_touched_[coord.x][coord.y] = true; // don't touch the first point

    raise_point_recurse(index, coord);

    // now go through the queue and, if the element hasn't been raised, raise it
    std::deque<ci::Vec2i>::iterator last_coord_it = bfs_last_coord_.begin();

    for (bfs_it = bfs_queue_.begin(); bfs_it != bfs_queue_.end(); ++bfs_it) {

        float last_height = post_points_[last_coord_it->x][last_coord_it->y].y;

        if (last_height < max_post_height_)
            post_points_[bfs_it->x][bfs_it->y].y = last_height + post_unit_height_;

        ++last_coord_it;
    }
}

void VoronoiSurface::raise_point_recurse(int index, ci::Vec2i coord) {
    if (point_ownership_[coord.x][coord.y] != index)
        return;

    ci::Vec2i c1, c2, c3, c4;
    bool b1, b2, b3, b4;
    b1 = b2 = b3 = b4 = false;
    c1 = ci::Vec2i(coord.x + 1, coord.y);
    c2 = ci::Vec2i(coord.x - 1, coord.y);
    c3 = ci::Vec2i(coord.x, coord.y + 1);
    c4 = ci::Vec2i(coord.x, coord.y - 1);

    // add all the neighboring points to the queue
    if (is_valid_coord(c1) && !grid_touched_[c1.x][c1.y]) {
        bfs_it = std::find(bfs_queue_.begin(), bfs_queue_.end(), c1);

        if (bfs_it == bfs_queue_.end()) {
            b1 = true;
            bfs_queue_.push_back(c1);
            bfs_last_coord_.push_back(coord);

            grid_touched_[c1.x][c1.y] = true;
        }
    }

    if (is_valid_coord(c2) && !grid_touched_[c2.x][c2.y]) {
        bfs_it = std::find(bfs_queue_.begin(), bfs_queue_.end(), c2);

        if (bfs_it == bfs_queue_.end()) {
            b2 = true;
            bfs_queue_.push_back(c2);
            bfs_last_coord_.push_back(coord);

            grid_touched_[c2.x][c2.y] = true;
        }
    }

    if (is_valid_coord(c3) && !grid_touched_[c3.x][c3.y]) {
        bfs_it = std::find(bfs_queue_.begin(), bfs_queue_.end(), c3);

        if (bfs_it == bfs_queue_.end()) {
            b3 = true;
            bfs_queue_.push_back(c3);
            bfs_last_coord_.push_back(coord);

            grid_touched_[c3.x][c3.y] = true;
        }
    }

    if (is_valid_coord(c4) && !grid_touched_[c4.x][c4.y]) {
        bfs_it = std::find(bfs_queue_.begin(), bfs_queue_.end(), c4);

        if (bfs_it == bfs_queue_.end()) {
            b4 = true;
            bfs_queue_.push_back(c4);
            bfs_last_coord_.push_back(coord);

            grid_touched_[c4.x][c4.y] = true;
        }
    }

    if (!(b1 || b2 || b3 || b4))
        return;

    std::vector<ci::Vec2i> coord_vec;

    if (b1) coord_vec.push_back(c1);
    if (b2) coord_vec.push_back(c2);
    if (b3) coord_vec.push_back(c3);
    if (b4) coord_vec.push_back(c4);

    while (!coord_vec.empty()) {
        int i = rand.randInt(coord_vec.size());

        raise_point_recurse(index, coord_vec[i]);

        std::vector<ci::Vec2i>::iterator pos = std::find(coord_vec.begin(),
            coord_vec.end(), coord_vec[i]);

        coord_vec.erase(pos);
    }

    /*
    if (b1)
        raise_point_recurse(index, c1);

    if (b2)
        raise_point_recurse(index, c2);

    if (b3)
        raise_point_recurse(index, c3);

    if (b4)
        raise_point_recurse(index, c4);
    */

}

void VoronoiSurface::update() {
    // nothing here
}

void VoronoiSurface::draw() {
    // nothing here
    ci::gl::color(ci::Color::white());
    glLineWidth(0.1f);

    glBegin(GL_LINES);

    int i = 0;
    for (LineVector::iterator it = surface_lines_.begin(); it != surface_lines_.end();
        ++it) {

        glColor3f(surface_colors_[i].r, surface_colors_[i].g, surface_colors_[i].b);

        glVertex3f(it->first.x, it->first.y, it->first.z);
        glVertex3f(it->second.x, it->second.y, it->second.z);

        ++i;
    }

    //glVertex3f(100, 100, 100);
    //glVertex3f(last_intersect_);

    glEnd();
}

bool VoronoiSurface::mouse_down(ci::app::MouseEvent evt) {
    if (!evt.isLeftDown())
        return false;

    ci::Ray r = Camera::instance().get_ray_from_screen_pos(evt.getPos());

    ci::Vec3f intersect = 
        CurveSketcher::instance().get_intersection_with_drawing_plane(r);

    //last_intersect_ = intersect;

    // find the closest index

    float min_dist = intersect.distance(ci::Vec3f::zero());
    int min_w = 0;
    int min_d = 0;

    for (int w = 0; w < grid_width_; ++w) {
        for (int d = 0; d < grid_depth_; ++d) {

            float dist = intersect.distance(ci::Vec3f(w * grid_unit_width_, 0.0f, 
                d * grid_unit_depth_));

            if (dist < min_dist) {
                min_dist = dist;
                min_w = w;
                min_d = d;
            }
        }
    }

    starting_coords_.push_back(ci::Vec2i(min_w, min_d));

    // reset
    reset();

    return false;
}

void VoronoiSurface::reset() {
    surface_lines_.clear();
    surface_colors_.clear();

    bfs_queue_.clear();
    bfs_last_coord_.clear();

    coord_colors_.clear();
    starting_coords_max_dists_.clear();

    point_ownership_.clear();
    post_points_.clear();
    grid_touched_.clear();

    init();

    load_random_colors();

    // calculate voronoi from points to know which point owns each post on the surface
    calculate_voronoi_ownership();
    
    calculate_max_dists();

    // march along lines grid, outward from points, raising posts
    raise_post_points();

    // stitch the post points into lines
    create_lines_from_grid();
}

}