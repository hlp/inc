
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
#include <cinder/Text.h>
#include <cinder/Rect.h>

#include <inc/Button.h>
#include <inc/Menu.h>
#include <incApp.h>

#if 0

namespace inc {

Button::Button(ci::Vec2f position, std::string label, Menu& menu) :
    position_(position), label_(label), menu_(menu) {
    pressed_ = false;
}

Button::~Button() {
    IncApp::instance().unregisterMouseDown(mouse_down_cb_id_);
    IncApp::instance().unregisterMouseUp(mouse_up_cb_id_);
}

void Button::setup() {
    mouse_down_cb_id_ = IncApp::instance().registerMouseDown(this, &Button::mouse_down);
    mouse_up_cb_id_ = IncApp::instance().registerMouseUp(this, &Button::mouse_up);

    ci::TextLayout layout;

    layout.setFont(ci::Font("Helvetica", 16.0f));
	layout.setColor(ci::Color::white());
    layout.addCenteredLine(label_);

    label_texture_ = ci::gl::Texture(layout.render(true));

    size_ = ci::Vec2f(label_texture_.getWidth(), label_texture_.getHeight());
    rect_ = ci::Rectf(ci::Vec2f::zero(), size_);
}

void Button::draw() {
    ci::gl::pushMatrices();
        ci::gl::translate(position_);

        /*
        if (pressed_)
            glColor4f(1.0f, 1.0f, 1.0f, 0.75f);
        else
            glColor4f(1.0f, 1.0f, 1.0f, 0.25f);

        ci::gl::drawSolidRect(rect_);
        */

        ci::gl::drawString(label_,ci::Vec2f::zero());

        /*
        ci::gl::enableDepthWrite( false );
		glEnable( GL_TEXTURE_2D );

        label_texture_.bind();

        glBegin(GL_QUADS);
	    glTexCoord2f( 0, 1 );
	    glVertex3f(0.0f, size_.y, 0.0f);
	    glTexCoord2f( 1, 1 );
	    glVertex3f(size_.x, size_.y, 0.0f);
	    glTexCoord2f( 1, 0 );
	    glVertex3f(size_.x, 0.0f, 0.0f);
	    glTexCoord2f( 0, 0 );
	    glVertex3f(0.0f, 0.0f, 0.0f);
	    glEnd();

        glDisable(GL_TEXTURE_2D);
        */
        
    ci::gl::popMatrices();
}

bool Button::mouse_down(ci::app::MouseEvent evt) {
    ci::Rectf global = rect_;
    global.offset(global_position());

    if (global.contains(evt.getPos())) {
        pressed_ = true;
        send_press_events();
    }

    return false;
}

void Button::send_press_events() {
    bool handled = false;

	for (ci::CallbackMgr<bool (ci::CallbackId)>::iterator it = press_mgr_.begin(); 
        it != press_mgr_.end(); ++it) {
        handled = (it->second)(it->first);
    }
}

bool Button::mouse_up(ci::app::MouseEvent) {
    pressed_ = false;

    return false;
}

ci::CallbackMgr<bool (ci::CallbackId)>& Button::press() {
    return press_mgr_;
}

ci::Vec2f Button::global_position() {
    return menu_.position() + position_;
}

ci::Vec2f Button::local_position() {
    return position_;
}

}

#endif