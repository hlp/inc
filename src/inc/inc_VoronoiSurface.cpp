
#include <utility>

#include <boost/date_time.hpp>

#include <cinder/gl/gl.h>
#include <cinder/Rand.h>

#include <inc/inc_VoronoiSurface.h>

namespace inc {

VoronoiSurface::VoronoiSurface() {
    max_post_height_ = 6.0f;
    post_unit_height_ = 1.0f;
    grid_width_ = 20;
    grid_depth_ = 50;
    grid_unit_width_ = 4.0f;
    grid_unit_depth_ = 4.0f;
    loft_res_ = 6; 
}

void VoronoiSurface::setup() {
    init();

    load_random_points(50);
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
    for (int i = 0; i < grid_width_; ++i) {
        point_ownership_.push_back(std::vector<int>());
        point_ownership_[i].resize(grid_depth_);
    }

    // set up post_points_ vector
    for (int i = 0; i < grid_width_; ++i) {
        post_points_.push_back(std::vector<ci::Vec3f>());
        for (int j = 0; j < grid_depth_; ++j) {
            post_points_[i].push_back(ci::Vec3f(i * grid_unit_width_,
                0.0f, j * grid_unit_depth_));
        }
    }

    // set up the grid_touched_ vector
    for (int i = 0; i < grid_width_; ++i) {
        grid_touched_.push_back(std::vector<bool>());
        for (int j = 0; j < grid_depth_; ++j) {
            grid_touched_[i].push_back(false);
        }
    }
}

void VoronoiSurface::load_random_points(int num_points) {
    boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
    boost::posix_time::ptime now(boost::posix_time::second_clock::universal_time());
    unsigned int seed = (now - epoch).total_seconds();
        
    rand.seed(seed);

    for (int i = 0; i < num_points; ++i) {
        starting_coords_.push_back(ci::Vec2i(rand.randInt(grid_width_),
            rand.randInt(grid_depth_)));
    }
}

void VoronoiSurface::load_random_colors() {
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

                float dist = ci::Vec2f(w * grid_width_, d * grid_depth_).distance(
                    ci::Vec2f(starting_coords_[i].x * grid_width_, 
                    starting_coords_[i].y * grid_depth_));

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
}

void VoronoiSurface::raise_post_points_lmap() {
    for (int i = 0; i < starting_coords_.size(); ++i) {

        for (int w = 0; w < grid_width_; ++w) {
            for (int d = 0; d < grid_depth_; ++d) {
                if (point_ownership_[w][d] != i)
                    continue;

                float dist = ci::Vec2f(w * grid_width_, d * grid_depth_).distance(
                    ci::Vec2f(starting_coords_[i].x * grid_width_, 
                    starting_coords_[i].y * grid_depth_));

                post_points_[w][d].y = ci::math<float>::floor(ci::lmap<float>(
                    dist, 0, starting_coords_max_dists_[i], 0, max_post_height_));
            }
        }

    }
}

void VoronoiSurface::bredth_first_raise_point(int index, ci::Vec2i coord) {

}

void VoronoiSurface::raise_point_recurse(ci::Vec2i coord) {
    // raise all neighboring points
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

    glEnd();
}

}