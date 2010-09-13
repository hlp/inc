
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

    std::for_each(widgets_.begin(), widgets_.end(),
        [] (WidgetPtr ptr) { ptr->add(); } );
}

void Menu::update() {
    std::for_each(widgets_.begin(), widgets_.end(),
        [] (WidgetPtr ptr) { ptr->update(); } );
}

void Menu::draw() {
    // nothing here
}

void Menu::add_widget(WidgetPtr ptr) {
    widgets_.push_back(ptr);
}



MainMenu::MainMenu() {
    instance_ = this;
}

MainMenu::~MainMenu() {
}

void MainMenu::setup() {
    interface_ = ci::params::InterfaceGl("Main_Menu", ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<float> > test_float = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Hello world"));

    test_float->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MainMenu::test_float_changed), this));

    add_widget(test_float);


    std::tr1::shared_ptr<GenericWidget<bool> > test_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Button"));

    test_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MainMenu::test_button_changed), this));

    add_widget(test_button);


    std::tr1::shared_ptr<GenericWidget<bool> > test_int = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Test int"));

    test_int->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MainMenu::test_int_changed), this));

    add_widget(test_int);

    
    /*
    save_dxf_cb_id_ = button_->press().registerCb(
        std::bind1st(std::mem_fun(&inc::MainMenu::save_dxf), this));
    
    add_widget(button_);
    */

    // this calls setup() on the widgets
    Menu::setup();
}

bool MainMenu::test_float_changed(float f) {
    ci::app::console() << f << std::endl;

    return false;
}

bool MainMenu::test_button_changed(bool b) {
    ci::app::console() << b << std::endl;

    return false;
}

bool MainMenu::test_int_changed(bool f) {
    ci::app::console() << "test changed " << f << std::endl;

    return false;
}

/*
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
*/

ci::params::InterfaceGl& MainMenu::interface() {
    return interface_;
}


MainMenu* MainMenu::instance_;

MainMenu& MainMenu::instance() {
    return *instance_;
}

}