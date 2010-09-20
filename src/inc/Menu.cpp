
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
#include <inc/SolidCreator.h>

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
    interface_ = ci::params::InterfaceGl("Main", ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > save_dxf_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Save DXF"));

    save_dxf_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MainMenu::save_dxf), this));

    add_widget(save_dxf_button);

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
    interface_ = ci::params::InterfaceGl("Mesh", ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > draw_mesh_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Draw Mesh Mode"));

    draw_mesh_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::CurveSketcher::activate_button_pressed), 
        &(CurveSketcher::instance())));

    add_widget(draw_mesh_button);

    std::tr1::shared_ptr<GenericWidget<bool> > bag_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Make circular mesh"));

    bag_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MeshMenu::create_bag), this));

    add_widget(bag_button);

    Menu::setup();
}

bool MeshMenu::create_bag(bool) {
    MeshCreator::instance().add_solid_bag(ci::Vec3f(0.0f, 2.0f, 0.0f),
        1.5f);

    return false;
}

MeshMenu* MeshMenu::instance_;

MeshMenu& MeshMenu::instance() {
    return *instance_;
}



SolidMenu::SolidMenu() {
    instance_ = this;

    matrix_w_ = 2;
    matrix_h_ = 2;
    matrix_d_ = 2;
}

SolidMenu::~SolidMenu() {
}

void SolidMenu::setup() {
    interface_ = ci::params::InterfaceGl("Solids", ci::Vec2i(380, 200));

    std::tr1::shared_ptr<GenericWidget<float> > set_gravity_button = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Set world gravity"));

    set_gravity_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::set_gravity), 
        this));

    add_widget(set_gravity_button);

    std::tr1::shared_ptr<GenericWidget<bool> > create_rigid_sphere_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create rigid sphere"));

    create_rigid_sphere_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_rigid_sphere), 
        this));

    add_widget(create_rigid_sphere_button);

    std::tr1::shared_ptr<GenericWidget<bool> > create_soft_sphere_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create soft sphere"));

    create_soft_sphere_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_soft_sphere), 
        this));

    add_widget(create_soft_sphere_button);

    std::tr1::shared_ptr<GenericWidget<bool> > create_linked_sphere_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create linked sphere"));

    create_linked_sphere_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_linked_spheres), 
        this));

    add_widget(create_linked_sphere_button);

    std::tr1::shared_ptr<GenericWidget<bool> > create_soft_sphere_matrix_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create soft sphere matrix"));

    create_soft_sphere_matrix_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_soft_sphere_matrix), 
        this));

    add_widget(create_soft_sphere_matrix_button);


    std::tr1::shared_ptr<GenericWidget<bool> > create_spring_matrix_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create rigid sphere spring matrix"));

    create_spring_matrix_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_rigid_sphere_spring_matrix), 
        this));

    add_widget(create_spring_matrix_button);

    std::tr1::shared_ptr<GenericWidget<bool> > create_rigid_matrix_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create rigid sphere matrix"));

    create_rigid_matrix_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_rigid_sphere_matrix), 
        this));

    add_widget(create_rigid_matrix_button);

    std::tr1::shared_ptr<GenericWidget<int> > set_w = 
        std::tr1::shared_ptr<GenericWidget<int> >(
        new GenericWidget<int>(*this, "Matrix width", &matrix_w_));

    add_widget(set_w);

    std::tr1::shared_ptr<GenericWidget<int> > set_h = 
        std::tr1::shared_ptr<GenericWidget<int> >(
        new GenericWidget<int>(*this, "Matrix height", &matrix_h_));

    add_widget(set_h);

    std::tr1::shared_ptr<GenericWidget<int> > set_d = 
        std::tr1::shared_ptr<GenericWidget<int> >(
        new GenericWidget<int>(*this, "Matrix depth", &matrix_d_));

    add_widget(set_d);

    Menu::setup();
}

bool SolidMenu::set_gravity(float grav) {
    SolidFactory::instance().set_gravity(grav);
    SolidFactory::instance().update_object_gravity();

    return false;
}

bool SolidMenu::create_rigid_sphere(bool) {
    SolidCreator::instance().create_rigid_sphere(ci::Vec3f(0.0f, 100.0f, 0.0f), 
        ci::Vec3f::one() * 3.0f);

    return false;
}

bool SolidMenu::create_soft_sphere(bool) {
    SolidCreator::instance().create_soft_sphere(ci::Vec3f(0.0f, 75.0f, 0.0f), 
        ci::Vec3f::one() * 3.0f);

    return false;
}

bool SolidMenu::create_linked_spheres(bool) {
    SolidCreator::instance().create_linked_spheres(ci::Vec3f(0.0f, 100.0f, 0.0f),
        ci::Vec3f::one() * 3.0f);

    return false;
}

bool SolidMenu::create_soft_sphere_matrix(bool) {
    SolidCreator::instance().create_sphere_matrix(ci::Vec3f(0.0f, 75.0f, 0.0f), 
        ci::Vec3f::one() * 3.0f,
        matrix_w_, matrix_h_, matrix_d_);

    return false;
}

bool SolidMenu::create_rigid_sphere_matrix(bool) {
    SolidCreator::instance().create_rigid_sphere_matrix(ci::Vec3f(0.0f, 100.0f, 0.0f), 
        ci::Vec3f::one() * 3.0f,
        matrix_w_, matrix_h_, matrix_d_);

    return false;
}

bool SolidMenu::create_rigid_sphere_spring_matrix(bool) {
    SolidCreator::instance().create_sphere_spring_matrix(ci::Vec3f(0.0f, 100.0f, 0.0f), 
        ci::Vec3f::one() * 3.0f, matrix_w_, matrix_h_, matrix_d_);

    return false;
}

SolidMenu* SolidMenu::instance_;

SolidMenu& SolidMenu::instance() {
    return *instance_;
}

}