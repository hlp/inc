
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
#include <inc/MeshCreator.h>
#include <inc/CurveSketcher.h>

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

ci::params::InterfaceGl& Menu::interface() {
    return interface_;
}



MainMenu::MainMenu() {
    instance_ = this;
}

MainMenu::~MainMenu() {
}

void MainMenu::setup() {
    interface_ = ci::params::InterfaceGl("Main_Menu", ci::Vec2i(200, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > save_dxf_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Save DXF"));

    save_dxf_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MainMenu::save_dxf), this));

    add_widget(save_dxf_button);

    std::tr1::shared_ptr<GenericWidget<bool> > bag_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Make mesh"));

    bag_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MainMenu::create_bag), this));

    add_widget(bag_button);

    // this calls setup() on the widgets and adds them to the tweek bar
    Menu::setup();
}

bool MainMenu::save_dxf(bool) {
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

bool MainMenu::create_bag(bool) {
    MeshCreator::instance().add_solid_bag(ci::Vec3f(0.0f, 2.0f, 0.0f),
        1.5f);

    return false;
}

MainMenu* MainMenu::instance_;

MainMenu& MainMenu::instance() {
    return *instance_;
}


MeshMenu::MeshMenu() {
    instance_ = this;
}

MeshMenu::~MeshMenu() {
}

void MeshMenu::setup() {
    interface_ = ci::params::InterfaceGl("Mesh_Menu", ci::Vec2i(200, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > draw_mesh_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Draw Mesh Mode"));

    draw_mesh_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::CurveSketcher::activate_button_pressed), 
        &(CurveSketcher::instance())));

    add_widget(draw_mesh_button);

    Menu::setup();
}

MeshMenu* MeshMenu::instance_;

MeshMenu& MeshMenu::instance() {
    return *instance_;
}

}