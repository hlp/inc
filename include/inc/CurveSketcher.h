
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

#include <vector>

#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/Ray.h>
#include <cinder/BSpline.h>
#include <cinder/Vector.h>
#include <cinder/app/MouseEvent.h>
#include <cinder/Surface.h>
#include <cinder/app/App.h>

#include <inc/Module.h>

namespace inc {

class ControlPoint;

class CurveSketcher : public Module {
public:
    CurveSketcher();
    virtual ~CurveSketcher();

    // Module override methods
    void setup();
    void update();
    void draw();

    bool mouse_down(ci::app::MouseEvent);
    bool mouse_drag(ci::app::MouseEvent);
    bool mouse_up(ci::app::MouseEvent);

    // callback for the button press
    bool activate_button_pressed(bool);
    bool* active_ptr(); // used for menu

    std::tr1::shared_ptr<ci::BSpline3f> current_spline();
    ci::Vec3f current_spline_center();

    bool invalid_curve();

    static CurveSketcher& instance();

    friend class ControlPoint; // so they can regenerate the bspline

private:
    void set_up_sketcher();
    void finish_sketcher();
    void generate_spline(bool closed); // called once per click
    void draw_control_points();
    bool check_control_point_intersection(ci::Ray,
        std::tr1::shared_ptr<ControlPoint>&);
    void create_new_control_point(ci::Ray);
    void deactivate_all_but_active();
    ci::Vec3f get_intersection_with_drawing_plane(ci::Ray);
    

    static CurveSketcher* instance_;
    bool active_;
    std::tr1::shared_ptr<ci::BSpline3f> current_spline_;

    bool refining_curve_;

    // specified by position and normal
    ci::Ray drawing_plane_;
    int degree_;

    // the spline is defined between 0 and 1
    float rendering_resolution_;
    float line_thickness_;
    ci::ColorA line_color_;

    std::tr1::shared_ptr<ControlPoint> active_point_;

    // there are as many control points as there are points_
    std::vector<std::tr1::shared_ptr<ControlPoint> > control_points_;

    ci::CallbackId mouse_down_cb_id_;
    ci::CallbackId mouse_drag_cb_id_;
    ci::CallbackId mouse_up_cb_id_;
};


class ControlPoint {
public:
    ControlPoint(ci::Vec3f pos, CurveSketcher&);
    ~ControlPoint();

    virtual void setup();
    // this draws the billboarded circle
    virtual void draw();

    // returns true if it is selected, false otherwise
    bool mouse_pressed(ci::Ray);
    bool mouse_dragged(ci::app::MouseEvent);
    bool mouse_released(ci::app::MouseEvent);

    void set_active(bool);

    // dircetions of the control points
    enum Direction {
        X_INCREASE,
        X_DECREASE,
        Z_INCREASE,
        Z_DECREASE
    };

    ci::Vec3f& position();
    

protected:
    // called when there's been a sucessful drag
    //virtual void move_point(ci::app::MouseEvent evt) = 0;

    CurveSketcher& sketcher_;

    ci::Vec3f position_;
    bool active_;
    float arrow_size_;

    float position_image_dim_; // what size to display the image
    float position_render_dim_; // what size to render the image
    ci::Surface position_image_;
    ci::gl::Texture position_texture_;

    ci::Surface active_image_;
    ci::gl::Texture active_texture_;

    ci::ColorA arrow_color_;
    float arrow_line_width_;
    float arrow_base_length_;
    float arrow_triangle_length_;
    float arrow_triangle_height_;

private:
    void draw_arrows();
};

/*
class HeightAdjuster : public ControlPoint {
public:
    HeightAdjuster();
    virtual ~HeightAdjuster();

    virtual void draw();

private:
    void draw_height_arrows();
};

*/


}