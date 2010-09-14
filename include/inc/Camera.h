
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

#include <cinder/params/Params.h>
#include <cinder/MayaCamUI.h>
#include <cinder/app/App.h>

#include <inc/Module.h>

namespace inc {

class Camera : public Module {
public:
    Camera();
    virtual ~Camera();
        
    void setup();
    void update();
    void draw();

    void set_matrices();

    virtual bool resize(int, int);
    virtual bool mouse_down(ci::app::MouseEvent);
    virtual bool mouse_drag(ci::app::MouseEvent);
    virtual bool mouse_up(ci::app::MouseEvent);
    virtual bool mouse_wheel(ci::app::MouseEvent);

    static Camera& instance();

    bool draw_interface();

private:
    void create_camera();
    void check_selection(ci::Vec2i);
    ci::Ray get_ray_from_screen_pos(ci::Vec2i pos);

    ci::MayaCamUI* cam_;
        
    float zoom_speed_;
    float far_clip_;

    static Camera* instance_;

    bool was_dragged_;
    ci::Vec2i down_pos_;

    ci::CallbackId mouse_down_cb_id_;
    ci::CallbackId mouse_drag_cb_id_;
    ci::CallbackId mouse_up_cb_id_;
    ci::CallbackId mouse_wheel_cb_id_;

    ci::Vec3f last_test_;
};

}