
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
#include <cinder/Vector.h>

#include <inc/Menu.h>
#include <incApp.h>
#include <inc/DxfSaver.h>
#include <inc/Manager.h>
#include <inc/Solid.h>
#include <inc/Widget.h>

namespace inc {

Menu::Menu() {
}

Menu::~Menu() {
#ifdef TRACE_DTORS
    ci::app::console() << "Deleting Menu" << std::endl;
#endif
}

void Menu::setup() {
    std::for_each(widgets_.begin(), widgets_.end(),
        [] (WidgetPtr ptr) { ptr->setup(); } );
}

void Menu::update() {
    // nothing here
}

void Menu::draw() {
    prepare_matrix();

    ci::gl::pushMatrices();
        ci::gl::translate(position_);

        std::for_each(widgets_.begin(), widgets_.end(),
            [] (WidgetPtr ptr) { ptr->draw(); } );

    ci::gl::popMatrices();
}

void Menu::add_widget(WidgetPtr ptr) {
    widgets_.push_back(ptr);
}

void Menu::prepare_matrix() {
    ci::gl::setMatricesWindow(IncApp::instance().getWindowSize());
}

const ci::Vec2f& Menu::position() const {
    return position_;
}




MainMenu::MainMenu() {
    instance_ = this;
}

MainMenu::~MainMenu() {
    button_->press().unregisterCb(save_dxf_cb_id_);
}

void MainMenu::setup() {
    // create a button, and register to receive its pressed events
    button_ = 
        std::tr1::shared_ptr<Button>(new Button(ci::Vec2f(50.0f, 50.0f),
        "Save DXF", *this));
    
    save_dxf_cb_id_ = button_->press().registerCb(
        std::bind1st(std::mem_fun(&inc::MainMenu::save_dxf), this));
    
    add_widget(button_);

    // this calls setup() on the widgets
    Menu::setup();
};

bool MainMenu::save_dxf(int) {
    ci::app::console() << "pressed" << std::endl;

    DxfSaver saver = DxfSaver("out.dxf");

    saver.begin();

    std::for_each(Manager::instance().solids().begin(),
        Manager::instance().solids().end(), 
        [&saver] (std::tr1::shared_ptr<Solid> solid) {
            solid->save(saver); 
            saver.add_layer();
        } );

    saver.end();

    return false;
}

MainMenu* MainMenu::instance_;

MainMenu& MainMenu::instance() {
    return *instance_;
}

}