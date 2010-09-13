
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

#include <memory>
#include <string>

#include <cinder/Vector.h>
#include <cinder/Function.h>

namespace inc {

class Menu;

class Widget {
public:
    Widget(Menu&, std::string label);
    virtual ~Widget();

    virtual void add() = 0; // add to the menu

    virtual void setup() { }
    virtual void update() { }

protected:
    Menu& menu_;
    std::string label_;
};

typedef std::tr1::shared_ptr<Widget> WidgetPtr;

// simple widget for changing float data information
class FloatWidget : public Widget {
public:
    FloatWidget(Menu&, std::string label, float* monitor = NULL);
    virtual ~FloatWidget();

    virtual void add();

    virtual void update();

    // use this to register objects to be called back
    ci::CallbackMgr<bool (float)>& value_changed();

private:
    void call_callbacks();

    ci::CallbackMgr<bool (float)> value_changed_;

    bool owner_;
    float* monitor_;
    float last_val_;
};


class ButtonWidget : public Widget {
public:
    ButtonWidget(Menu&, std::string label, bool* monitor = NULL);
    virtual ~ButtonWidget();

    virtual void add();

    virtual void update();

    // use this to register objects to be called back
    ci::CallbackMgr<bool (bool)>& value_changed();

private:
    void call_callbacks();

    ci::CallbackMgr<bool (bool)> value_changed_;

    bool owner_;
    bool* monitor_;
    bool last_val_;
};

}