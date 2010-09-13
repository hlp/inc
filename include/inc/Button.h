
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

#include <string>

#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/Function.h>
#include <cinder/app/MouseEvent.h>
#include <cinder/Vector.h>

#include <inc/Widget.h>

namespace inc {

class Menu;

class Button : public Widget {
public:
    Button(ci::Vec2f position, std::string label, Menu& parent);
    virtual ~Button();

    void setup();
    void draw();

    bool mouse_down(ci::app::MouseEvent);
    bool mouse_up(ci::app::MouseEvent);

    ci::Vec2f global_position();
    ci::Vec2f local_position();

    ci::CallbackMgr<bool (ci::CallbackId)>& press();

private:
    void send_press_events(); // call registered objects

    ci::Vec2f position_;
    ci::Vec2f size_;
    ci::Rectf rect_;
    std::string label_;
    Menu& menu_;

    ci::CallbackId mouse_down_cb_id_;
    ci::CallbackId mouse_up_cb_id_;

    bool pressed_;
    ci::gl::Texture label_texture_;

    ci::CallbackMgr<bool (ci::CallbackId)> press_mgr_;
};

}