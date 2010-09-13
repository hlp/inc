
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

#include <inc/Widget.h>
#include <inc/Menu.h>

namespace inc {

Widget::Widget(Menu& menu, std::string label) 
    : menu_(menu), label_(label) {
}

Widget::~Widget() {
}


FloatWidget::FloatWidget(Menu& menu, std::string label, float* monitor) 
    : Widget(menu, label) {

    if (monitor == NULL) {
        owner_ = true;
        monitor_ = new float();
        *monitor_ = 0.0f;
    } else {
        owner_ = false;
        monitor_ = monitor;
    }

    last_val_ = *monitor_;
}

FloatWidget::~FloatWidget() {
    if (owner_)
        delete monitor_;
}

ci::CallbackMgr<bool (float)>& FloatWidget::value_changed() {
    return value_changed_;
}

void FloatWidget::add() {
    menu_.interface().addParam(label_, monitor_);
}

void FloatWidget::update() {
    if (last_val_ != *monitor_) {
        call_callbacks();
        last_val_ = *monitor_;
    }
}

void FloatWidget::call_callbacks() {
    bool handled = false;

	for (ci::CallbackMgr<bool (float)>::iterator it = value_changed_.begin(); 
        it != value_changed_.end(); ++it) {
        handled = (it->second)(*monitor_);
    }
}


ButtonWidget::ButtonWidget(Menu& menu, std::string label, bool* monitor) :
    Widget(menu, label) {
    if (monitor == NULL) {
        owner_ = true;
        monitor_ = new bool();
        *monitor_ = false;
    } else {
        owner_ = false;
        monitor_ = monitor;
    }

    last_val_ = *monitor_;
}

ButtonWidget::~ButtonWidget() {
    if (owner_)
        delete monitor_;
}

ci::CallbackMgr<bool (bool)>& ButtonWidget::value_changed() {
    return value_changed_;
}

void ButtonWidget::add() {
    menu_.interface().addParam(label_, monitor_);
}

void ButtonWidget::update() {
    if (last_val_ != *monitor_) {
        call_callbacks();
        last_val_ = *monitor_;
    }
}

void ButtonWidget::call_callbacks() {
    bool handled = false;

	for (ci::CallbackMgr<bool (bool)>::iterator it = value_changed_.begin(); 
        it != value_changed_.end(); ++it) {
        handled = (it->second)(*monitor_);
    }
}

}