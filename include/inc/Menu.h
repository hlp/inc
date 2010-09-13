
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

#include <deque>

#include <cinder/app/KeyEvent.h>
#include <cinder/app/App.h>

#include <inc/Module.h>
#include <inc/Widget.h>
#include <inc/Button.h>

namespace inc {

class Menu : public Module {
public:
    Menu();
    virtual ~Menu();

    virtual void setup();
    virtual void update();
    virtual void draw();

    void add_widget(WidgetPtr);

    const ci::Vec2f& position() const;

private:
    std::deque<WidgetPtr> widgets_;
    ci::Vec2f position_;

    ci::CallbackId key_down_cb_id_;

    // sets up the OpenGL matrix for 2D drawing
    void prepare_matrix();
};

class MainMenu: public Menu {
public:
    MainMenu();
    virtual ~MainMenu();

    virtual void setup();

    bool save_dxf(int);
    ci::CallbackId save_dxf_cb_id_;

    MainMenu& instance();

private:
    static MainMenu* instance_;

    std::tr1::shared_ptr<Button> button_;
};

}