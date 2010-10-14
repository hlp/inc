
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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

#include <cinder/gl/gl.h>
#include <cinder/Vector.h>
#include <cinder/cairo/Cairo.h>
#include <cinder/gl/Texture.h>
#include <cinder/Surface.h>
#include <cinder/app/MouseEvent.h>
#include <cinder/ImageIo.h>

#include <inc/inc_Menu.h>
#include <incApp.h>
#include <inc/inc_DxfSaver.h>
#include <inc/inc_Manager.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Widget.h>
#include <inc/inc_MeshCreator.h>
#include <inc/inc_CurveSketcher.h>
#include <inc/inc_SolidCreator.h>
#include <inc/inc_Renderer.h>
#include <inc/inc_Origin.h>
#include <inc/inc_MeshNetwork.h>

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
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 50));

    // this calls setup() on the widgets and adds them to the tweek bar
    Menu::setup();
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
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 200));

    // TODO: decide if I want to keep this method. If not, remove it
    /*
    std::tr1::shared_ptr<GenericWidget<bool> > bag_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Make circular mesh"));

    bag_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MeshMenu::create_bag), this));

    add_widget(bag_button);
    */

    std::tr1::shared_ptr<GenericWidget<float> > kDF = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Dynamic friction coefficient",
        SolidFactory::instance().kDF_ptr(), "step=0.01 min=0 max=1"));

    kDF->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidFactory::adjust_kDF), 
        SolidFactory::instance_ptr()));

    add_widget(kDF);

    std::tr1::shared_ptr<GenericWidget<float> > kDP = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Damping coefficient",
        SolidFactory::instance().kDP_ptr(), "step=0.01 min=0 max=1"));

    kDP->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidFactory::adjust_kDP), 
        SolidFactory::instance_ptr()));

    add_widget(kDP);

    std::tr1::shared_ptr<GenericWidget<float> > kDG =
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Drag coefficient",
        SolidFactory::instance().kDG_ptr(), "step=0.1 min=0"));

    kDG->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidFactory::adjust_kDG), 
        SolidFactory::instance_ptr()));

    add_widget(kDG);

    std::tr1::shared_ptr<GenericWidget<float> > kPR = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Pressure coefficient",
        SolidFactory::instance().kPR_ptr(), "step=0.01"));

    kPR->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidFactory::adjust_kPR), 
        SolidFactory::instance_ptr()));

    add_widget(kPR);

    std::tr1::shared_ptr<GenericWidget<float> > kMT = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Pose matching coefficient",
        SolidFactory::instance().kMT_ptr(), "step=0.01 min=0 max=1"));

    kMT->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidFactory::adjust_kMT), 
        SolidFactory::instance_ptr()));

    add_widget(kMT);
    

    Menu::setup();
}

bool MeshMenu::create_bag(bool) {
    MeshCreator::instance().add_circle_mesh(ci::Vec3f(0.0f, 2.0f, 0.0f),
        1.5f);

    return false;
}

MeshMenu* MeshMenu::instance_;

MeshMenu& MeshMenu::instance() {
    return *instance_;
}



SolidMenu::SolidMenu() {
    instance_ = this;

    matrix_w_ = 1;
    matrix_h_ = 10;
    matrix_d_ = 1;

    sphere_radius_ = 3.0f;
}

SolidMenu::~SolidMenu() {
}

void SolidMenu::setup() {
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(380, 250));

    std::tr1::shared_ptr<GenericWidget<float> > set_gravity_button = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "World gravity",
        SolidFactory::instance().gravity_ptr(), "step=0.05"));

    set_gravity_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidFactory::set_gravity), 
        SolidFactory::instance_ptr()));

    add_widget(set_gravity_button);

    std::tr1::shared_ptr<GenericWidget<float> > sphere_radius_button = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "New sphere radius",
        &sphere_radius_, "step=0.1 min=0.1"));

    add_widget(sphere_radius_button);

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

    /*
    std::tr1::shared_ptr<GenericWidget<bool> > create_linked_sphere_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create linked sphere"));

    create_linked_sphere_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_linked_spheres), 
        this));

    add_widget(create_linked_sphere_button);
    */

    std::tr1::shared_ptr<GenericWidget<bool> > create_soft_sphere_matrix_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create soft sphere matrix"));

    create_soft_sphere_matrix_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_soft_sphere_matrix), 
        this));

    add_widget(create_soft_sphere_matrix_button);

    /*
    std::tr1::shared_ptr<GenericWidget<bool> > create_spring_matrix_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create rigid sphere spring matrix"));

    create_spring_matrix_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_rigid_sphere_spring_matrix), 
        this));

    add_widget(create_spring_matrix_button);
    */

    /*
    std::tr1::shared_ptr<GenericWidget<bool> > create_rigid_matrix_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create rigid sphere matrix"));

    create_rigid_matrix_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::SolidMenu::create_rigid_sphere_matrix), 
        this));

    add_widget(create_rigid_matrix_button);
    */

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

    std::tr1::shared_ptr<GenericWidget<float> > sphere_kLST = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Linear stiffness coefficient", 
        SolidFactory::instance().sphere_kLST_ptr(), "step=0.01 min=0 max=1"));

    add_widget(sphere_kLST);

    std::tr1::shared_ptr<GenericWidget<float> > sphere_kVST = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Volume stiffness coefficient", 
        SolidFactory::instance().sphere_kVST_ptr(), "step=0.01 min=0 max=1"));

    add_widget(sphere_kVST);

    std::tr1::shared_ptr<GenericWidget<float> > sphere_kDF = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Dynamic friction coefficient", 
        SolidFactory::instance().sphere_kDF_ptr(), "step=0.01 min=0 max=1"));

    add_widget(sphere_kDF);

    std::tr1::shared_ptr<GenericWidget<float> > sphere_kDP = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Damping coefficient", 
        SolidFactory::instance().sphere_kDP_ptr(), "step=0.01 min=0 max=1"));

    add_widget(sphere_kDP);

    std::tr1::shared_ptr<GenericWidget<float> > sphere_kPR = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Pressure coefficient", 
        SolidFactory::instance().sphere_kPR_ptr(), "step=1"));

    add_widget(sphere_kPR);

    std::tr1::shared_ptr<GenericWidget<float> > sphere_total_mass = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Sphere total mass", 
        SolidFactory::instance().sphere_total_mass_ptr(), "step=0.1 min=0.1"));

    add_widget(sphere_total_mass);

    Menu::setup();
}

bool SolidMenu::set_gravity(float grav) {
    SolidFactory::instance().set_gravity(grav);
    SolidFactory::instance().update_object_gravity();

    return false;
}

bool SolidMenu::create_rigid_sphere(bool) {
    ci::Vec3f pos = SolidCreator::instance().creation_point();

    SolidCreator::instance().create_rigid_sphere(pos, 
        ci::Vec3f::one() * sphere_radius_);

    return false;
}

bool SolidMenu::create_soft_sphere(bool) {
    //ci::Vec3f pos = CurveSketcher::instance().current_spline_center();
    ci::Vec3f pos = SolidCreator::instance().creation_point();

    SolidCreator::instance().create_soft_sphere(pos, ci::Vec3f::one() * sphere_radius_);

    return false;
}

bool SolidMenu::create_linked_spheres(bool) {
    SolidCreator::instance().create_linked_spheres(ci::Vec3f(0.0f, 100.0f, 0.0f),
        ci::Vec3f::one() * sphere_radius_);

    return false;
}

bool SolidMenu::create_soft_sphere_matrix(bool) {
    //ci::Vec3f pos = CurveSketcher::instance().current_spline_center();
    ci::Vec3f pos = SolidCreator::instance().creation_point();

    SolidCreator::instance().create_sphere_matrix(pos, ci::Vec3f::one() * sphere_radius_,
        matrix_w_, matrix_h_, matrix_d_);

    return false;
}

bool SolidMenu::create_rigid_sphere_matrix(bool) {
    SolidCreator::instance().create_rigid_sphere_matrix(ci::Vec3f(0.0f, 100.0f, 0.0f), 
        ci::Vec3f::one() * sphere_radius_,
        matrix_w_, matrix_h_, matrix_d_);

    return false;
}

bool SolidMenu::create_rigid_sphere_spring_matrix(bool) {
    SolidCreator::instance().create_sphere_spring_matrix(ci::Vec3f(0.0f, 100.0f, 0.0f), 
        ci::Vec3f::one() * sphere_radius_, matrix_w_, matrix_h_, matrix_d_);

    return false;
}

SolidMenu* SolidMenu::instance_;

SolidMenu& SolidMenu::instance() {
    return *instance_;
}


// having a dynamic window box could get a bit hairy, hence these static methods
void ForceMenu::add_menu(Solid& solid) {
    // it would be good to change this...
    IncApp::instance().force_menu_ = 
        std::tr1::shared_ptr<ForceMenu>(new ForceMenu(solid));

    Manager::instance().add_module(IncApp::instance().force_menu_);

    IncApp::instance().force_menu_->setup();
}

void ForceMenu::remove_menu() {
    Manager::instance().remove_module(IncApp::instance().force_menu_);
    IncApp::instance().force_menu_.reset();
}

ForceMenu::ForceMenu(Solid& solid) : target_solid_(solid) {
    instance_ = this;
}

ForceMenu::~ForceMenu() {
}

void ForceMenu::setup() {
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 175));

    std::tr1::shared_ptr<GenericWidget<ci::Vec3f> > change_force_button =
        std::tr1::shared_ptr<GenericWidget<ci::Vec3f> >(
        new GenericWidget<ci::Vec3f>(*this, "Set Object Force", target_solid_.force_ptr()));

    change_force_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::ForceMenu::force_changed), 
        this));

    add_widget(change_force_button);

    Menu::setup();
}

// really you're setting a const 
bool ForceMenu::force_changed(ci::Vec3f vel) {
    // notify the object this is the menu for
    target_solid_.set_force(vel);

    return false;
}

ForceMenu* ForceMenu::instance_;

ForceMenu& ForceMenu::instance() {
    return *instance_;
}


//////////////////////
// DrawMesh

void DrawMeshMenu::setup() {
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > draw_mesh_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Draw Mesh Curve Mode",
        CurveSketcher::instance().active_ptr()));

    draw_mesh_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::CurveSketcher::activate_button_pressed), 
        &(CurveSketcher::instance())));

    add_widget(draw_mesh_button);

    std::tr1::shared_ptr<GenericWidget<float> > mesh_height = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Mesh height",
        MeshCreator::instance().mesh_scale_ptr(), "step=0.1 min=0.1"));

    mesh_height->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MeshCreator::adjust_mesh_scale), 
        MeshCreator::instance_ptr()));

    add_widget(mesh_height);

    std::tr1::shared_ptr<GenericWidget<int> > arch_res = 
        std::tr1::shared_ptr<GenericWidget<int> >(
        new GenericWidget<int>(*this, "Mesh arch resolution",
        MeshCreator::instance().arch_resolution_ptr(), "step=1 min=4"));

    arch_res->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MeshCreator::adjust_arch_resolution), 
        MeshCreator::instance_ptr()));

    add_widget(arch_res);

    std::tr1::shared_ptr<GenericWidget<int> > slice_res = 
        std::tr1::shared_ptr<GenericWidget<int> >(
        new GenericWidget<int>(*this, "Mesh slice resolution",
        MeshCreator::instance().slice_resolution_ptr(), "step=1 min=4"));

    slice_res->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MeshCreator::adjust_slice_resolution), 
        MeshCreator::instance_ptr()));

    add_widget(slice_res);

    Menu::setup();
}



/////////////////////////////////////
// TripodMeshMenu

void TripodMeshMenu::setup() {
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > create_mesh = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create Tripod Mesh"));

    create_mesh->value_changed().registerCb(
        std::bind1st(std::mem_fun(&TripodMeshMenu::create_mesh), 
        this));

    add_widget(create_mesh);

    std::tr1::shared_ptr<GenericWidget<int> > set_legs = 
        std::tr1::shared_ptr<GenericWidget<int> >(
        new GenericWidget<int>(*this, "Number of legs", 
        MeshCreator::instance().tripod_legs_ptr(), "min=1 max=5"));

    add_widget(set_legs);

    std::tr1::shared_ptr<GenericWidget<float> > mesh_scale = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Mesh scale",
        MeshCreator::instance().tripod_mesh_scale_ptr(), "step=0.1 min=0.1"));

    mesh_scale->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MeshCreator::adjust_tripod_mesh_scale), 
        MeshCreator::instance_ptr()));

    add_widget(mesh_scale);

    Menu::setup();
}

bool TripodMeshMenu::create_mesh(bool) {
    MeshCreator::instance().add_tripod_mesh();

    return false;
}



///////////////////////////////////////
// AnemoneMeshMenu

void AnemoneMeshMenu::setup() {
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > create_mesh = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create Anemone Mesh"));

    create_mesh->value_changed().registerCb(
        std::bind1st(std::mem_fun(&AnemoneMeshMenu::create_mesh), 
        this));

    add_widget(create_mesh);

    std::tr1::shared_ptr<GenericWidget<int> > set_arms = 
        std::tr1::shared_ptr<GenericWidget<int> >(
        new GenericWidget<int>(*this, "Number of arms", 
        MeshCreator::instance().anemone_legs_ptr(), "min=1 max=5"));

    add_widget(set_arms);

    std::tr1::shared_ptr<GenericWidget<float> > mesh_scale = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Mesh scale",
        MeshCreator::instance().anemone_mesh_scale_ptr(), "step=0.1 min=0.1"));

    mesh_scale->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::MeshCreator::adjust_anemone_mesh_scale), 
        MeshCreator::instance_ptr()));

    add_widget(mesh_scale);

    Menu::setup();
}

bool AnemoneMeshMenu::create_mesh(bool) {
    MeshCreator::instance().add_anemone_mesh();

    return false;
}


void MeshNetworkMenu::setup() {
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > create_mesh = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Create mesh network"));

    create_mesh->value_changed().registerCb(
        std::bind1st(std::mem_fun(&MeshNetworkMenu::create_network), 
        this));

    add_widget(create_mesh);

    Menu::setup();
}

bool MeshNetworkMenu::create_network(bool) {
    MeshCreator::instance().mesh_network()->create_tube_union();

    return false;
}


void DisplayMenu::setup() {
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > alpha_blending = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "OpenGL enable alpha blending",
        Renderer::instance().enable_alpha_blending_ptr()));

    add_widget(alpha_blending);

    std::tr1::shared_ptr<GenericWidget<bool> > depth_read = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "OpenGL enable depth read",
        Renderer::instance().enable_depth_read_ptr()));

    add_widget(depth_read);

    std::tr1::shared_ptr<GenericWidget<bool> > depth_write = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "OpenGL enable depth write",
        Renderer::instance().enable_depth_write_ptr()));

    add_widget(depth_write);

    std::tr1::shared_ptr<GenericWidget<ci::ColorA> > base_color = 
        std::tr1::shared_ptr<GenericWidget<ci::ColorA> >(
        new GenericWidget<ci::ColorA>(*this, "Mesh base gradient color",
        Renderer::instance().base_color_ptr()));

    add_widget(base_color);

    std::tr1::shared_ptr<GenericWidget<ci::ColorA> > top_color = 
        std::tr1::shared_ptr<GenericWidget<ci::ColorA> >(
        new GenericWidget<ci::ColorA>(*this, "Mesh top gradient color",
        Renderer::instance().top_color_ptr()));

    add_widget(top_color);

    std::tr1::shared_ptr<GenericWidget<ci::ColorA> > line_color = 
        std::tr1::shared_ptr<GenericWidget<ci::ColorA> >(
        new GenericWidget<ci::ColorA>(*this, "Mesh line color",
        Renderer::instance().line_color_ptr()));

    add_widget(line_color);

    std::tr1::shared_ptr<GenericWidget<float> > thickness = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Mesh line thickness",
        Renderer::instance().line_thickness_ptr(), "min=0.0 step=0.01"));

    add_widget(thickness);

    /*
    std::tr1::shared_ptr<GenericWidget<ci::ColorA> > solids_base_color = 
        std::tr1::shared_ptr<GenericWidget<ci::ColorA> >(
        new GenericWidget<ci::ColorA>(*this, "Solids base gradient color",
        Renderer::instance().solids_base_color_ptr()));

    add_widget(solids_base_color);

    std::tr1::shared_ptr<GenericWidget<ci::ColorA> > solids_top_color = 
        std::tr1::shared_ptr<GenericWidget<ci::ColorA> >(
        new GenericWidget<ci::ColorA>(*this, "Solids top gradient color",
        Renderer::instance().solids_top_color_ptr()));

    add_widget(solids_top_color);

    std::tr1::shared_ptr<GenericWidget<ci::ColorA> > solids_line_color = 
        std::tr1::shared_ptr<GenericWidget<ci::ColorA> >(
        new GenericWidget<ci::ColorA>(*this, "Solids line color",
        Renderer::instance().solids_line_color_ptr()));

    add_widget(solids_line_color);

    std::tr1::shared_ptr<GenericWidget<float> > solids_thickness = 
        std::tr1::shared_ptr<GenericWidget<float> >(
        new GenericWidget<float>(*this, "Solids line thickness",
        Renderer::instance().solids_line_thickness_ptr()));

    add_widget(solids_thickness);
    */

    std::tr1::shared_ptr<GenericWidget<bool> > grid = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Draw origin and grid",
        Origin::instance().draw_grid_ptr()));

    add_widget(grid);

    Menu::setup();
}




FileMenu::FileMenu() {
    image_counter_ = 0;
    high_res_image_width_ = 5000;
    file_name_ = "mosball_";
    save_uuid_ = true;
}

void FileMenu::setup() {
    interface_ = ci::params::InterfaceGl(name(), ci::Vec2i(300, 200));

    std::tr1::shared_ptr<GenericWidget<bool> > save_dxf_button = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Save DXF"));

    save_dxf_button->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::FileMenu::save_dxf), this));

    add_widget(save_dxf_button);

    std::tr1::shared_ptr<GenericWidget<bool> > save = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Save screen"));

    save->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::FileMenu::save_image), this));

    add_widget(save);

    std::tr1::shared_ptr<GenericWidget<bool> > save_high_res = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Save high res image"));

    save_high_res->value_changed().registerCb(
        std::bind1st(std::mem_fun(&inc::FileMenu::save_high_res), this));

    add_widget(save_high_res);

    std::tr1::shared_ptr<GenericWidget<int> > high_res_width = 
        std::tr1::shared_ptr<GenericWidget<int> >(
        new GenericWidget<int>(*this, "High res image width", 
        &high_res_image_width_, "min=2 step=1"));

    add_widget(high_res_width);

    std::tr1::shared_ptr<GenericWidget<std::string> > name = 
        std::tr1::shared_ptr<GenericWidget<std::string> >(
        new GenericWidget<std::string>(*this, "Image name", &file_name_));

    add_widget(name);

    std::tr1::shared_ptr<GenericWidget<bool> > save_uuid = 
        std::tr1::shared_ptr<GenericWidget<bool> >(
        new GenericWidget<bool>(*this, "Use UUID not numbers",
        &save_uuid_));

    add_widget(save_uuid);

    Menu::setup();
}

bool FileMenu::save_image(bool) {
    ci::writeImage(get_file_name() + ".png", IncApp::instance().copyWindowSurface());

    return true;
}

bool FileMenu::save_high_res(bool) {
    Renderer::instance().save_image(high_res_image_width_, get_file_name() + ".png");

    return true;
}

std::string FileMenu::get_uuid() {
    boost::uuids::random_generator gen;
    boost::uuids::uuid id = gen();
    std::string uuid_string = boost::lexical_cast<std::string>(id);

    return uuid_string;
}

std::string FileMenu::get_file_name() {
    std::string output_name;

    if (save_uuid_) {
        output_name = file_name_ + get_uuid();
    } else {
        std::ostringstream ss;
        ss << image_counter_;
        output_name = file_name_ + ss.str();
        ++image_counter_;
    }

    return output_name;
}

bool FileMenu::save_dxf(bool) {
    DxfSaver saver = DxfSaver(get_file_name() + ".dxf");

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



/////////////////////////////////////
// MenuManager

MenuManager::MenuManager() {
    instance_ = this;

    selected_menu_ = MAIN;

    x_origin_ = 10;
    y_origin_ = 17;
    x_spacing_ = 67;
    y_spacing_ = 22; // the height

    hovering_over_menu_ = false;
}

MenuManager::~MenuManager() {
    IncApp::instance().unregisterResize(resize_cb_id_);
    IncApp::instance().unregisterMouseMove(mouse_moved_cb_id_);

    menus_.clear();
}

void MenuManager::setup() {
    resize_cb_id_ = IncApp::instance().registerResize(this,
        &MenuManager::resize);
    mouse_moved_cb_id_ = IncApp::instance().registerMouseMove(this, 
        &MenuManager::mouse_moved);

    create_menus();
    setup_menus();

    menu_rect_ = ci::Rectf(ci::Vec2f::zero(),
        ci::Vec2f(x_origin_ + x_spacing_, 
        y_spacing_ * menus_.size()));

    for (int i = 0; i < menus_.size(); ++i) {
        ci::Rectf temp_rect = 
            ci::Rectf(ci::Vec2f(0.0f, y_spacing_ * i),
            ci::Vec2f(x_origin_ + x_spacing_, y_spacing_ * (i+1)));

        submenu_rects_.push_back(temp_rect);
    }

    // create the create the tab menu
    create_menu_texture();

    hide_all_menus();
    show_menu(selected_menu_);
}

void MenuManager::position_menu() {
    menu_pos_ = 
        ci::Vec2f(IncApp::instance().getWindowWidth() - menu_rect_.getWidth(),
        0.0f);
}

void MenuManager::create_menus() {
    menus_.push_back(std::tr1::shared_ptr<Menu>(new MainMenu()));
    menus_.push_back(std::tr1::shared_ptr<Menu>(new MeshMenu()));
    menus_.push_back(std::tr1::shared_ptr<Menu>(new DrawMeshMenu()));
    menus_.push_back(std::tr1::shared_ptr<Menu>(new TripodMeshMenu()));
    menus_.push_back(std::tr1::shared_ptr<Menu>(new AnemoneMeshMenu()));
    menus_.push_back(std::tr1::shared_ptr<Menu>(new MeshNetworkMenu()));
    menus_.push_back(std::tr1::shared_ptr<Menu>(new SolidMenu()));
    menus_.push_back(std::tr1::shared_ptr<Menu>(new DisplayMenu()));
    menus_.push_back(std::tr1::shared_ptr<Menu>(new FileMenu()));
}

void MenuManager::setup_menus() {
    std::for_each(menus_.begin(), menus_.end(), 
        [=] (std::tr1::shared_ptr<Menu> menu) {
            menu->setup();
    } );
}

void MenuManager::create_menu_texture() {
    std::string font_name = "Lucida Console";
    int font_size = 12;

    int x_origin = x_origin_;
    int y_origin = y_origin_;
    int x_spacing = x_spacing_;
    int y_spacing = y_spacing_; // the height

    ci::cairo::SurfaceImage base(x_origin + x_spacing, 
        y_spacing * menus_.size(), true);
    ci::cairo::Context ctx(base);

    ci::Font font(font_name, font_size);
    ctx.setSourceRgba(0.1f, 0.1f, 0.1f, 0.8f);
    ctx.rectangle(0.0, 0.0, x_origin + x_spacing, y_spacing * menus_.size());
    ctx.fill();
    ctx.setAntiAlias(ci::cairo::ANTIALIAS_NONE);
    ctx.setFont(font);
    ctx.setFontSize(font_size);
    ctx.setSourceRgba(1.0f, 1.0f, 1.0f, 1.0f);

    for (int i = 0; i < menus_.size(); ++i) {
        ctx.moveTo(x_origin, y_spacing * i + y_origin);
        ctx.showText(menus_[i]->name());
    }

    ci::Surface tabs_surface = base.getSurface();
    tabs_texture_ = ci::gl::Texture(tabs_surface);

    for (int i = 0; i < menus_.size(); ++i) {
        ci::cairo::SurfaceImage roll_base(x_origin + x_spacing, 
        y_spacing * menus_.size(), true);
        ci::cairo::Context roll_ctx(roll_base);

        roll_ctx.setSourceRgba(0.1f, 0.1f, 0.1f, 0.8f);
        roll_ctx.rectangle(0.0, 0.0, x_origin + x_spacing, y_spacing * menus_.size());
        roll_ctx.fill();
        roll_ctx.setSourceRgba(0.3f, 0.3f, 0.3f, 1.0f);

        roll_ctx.rectangle(submenu_rects_[i]);
        roll_ctx.fill();

        ci::Font font(font_name, font_size);
        roll_ctx.setAntiAlias(ci::cairo::ANTIALIAS_NONE);
        roll_ctx.setFont(font);
        roll_ctx.setFontSize(font_size);
        roll_ctx.setSourceRgba(1.0f, 1.0f, 1.0f, 1.0f);

        for (int j = 0; j < menus_.size(); ++j) {
            roll_ctx.moveTo(x_origin, y_spacing * j + y_origin);
            roll_ctx.showText(menus_[j]->name());
        }

        rollover_textures_.push_back(ci::gl::Texture(roll_base.getSurface()));
    }
}

void MenuManager::update() {
    std::for_each(menus_.begin(), menus_.end(),
        [=] (std::tr1::shared_ptr<Menu> menu) {
            menu->update();
    } );
}

void MenuManager::draw() {
    std::for_each(menus_.begin(), menus_.end(),
        [=] (std::tr1::shared_ptr<Menu> menu) {
            menu->draw();
    } );
}

void MenuManager::delayed_draw() {
    // draw the tabs
    draw_menu_texture();
}

void MenuManager::draw_menu_texture() {
    // reset matrix for 2D drawing
    ci::gl::setMatricesWindow(IncApp::instance().getWindowSize());

    ci::gl::color(ci::Color::white());

    if (hovering_over_menu_)
        ci::gl::draw(rollover_textures_[selected_menu_], menu_pos_);
    else
        ci::gl::draw(tabs_texture_, menu_pos_);
}

bool MenuManager::resize(ci::app::ResizeEvent) {
    position_menu();

    return false;
}

bool MenuManager::mouse_moved(ci::app::MouseEvent evt) {
    // check if the cursor is in a menu != selected_menu_
    // if so, show that menu, and hide all others
    if (!is_inside_menu(evt.getPos())) {
        hovering_over_menu_ = false;
        return false;
    }

    hovering_over_menu_ = true;

    SelectedMenu current_hover = get_hover_menu(evt.getPos());

    if (current_hover != selected_menu_) {
        selected_menu_ = current_hover;
        hide_all_menus();
        show_menu(selected_menu_);
    }
    
    return false;
}

bool MenuManager::is_inside_menu(ci::Vec2i pos) {
    ci::Rectf temp_rect(menu_rect_);
    temp_rect.offset(menu_pos_);
    return temp_rect.contains(pos);
}

MenuManager::SelectedMenu MenuManager::get_hover_menu(ci::Vec2i pos) {
    for (int i = 0; i < menus_.size(); ++i) {
        ci::Rectf temp_rect(submenu_rects_[i]);
        temp_rect.offset(menu_pos_);
        if (temp_rect.contains(pos))
            return (SelectedMenu)i;
    }

    return MAIN;
}

void MenuManager::hide_all_menus() {
    // iterate over the menu containter, and hide all the menu
    std::for_each(menus_.begin(), menus_.end(), 
        [=] (std::tr1::shared_ptr<Menu> menu) {
            menu->interface().hide();
    } );
}

void MenuManager::show_menu(SelectedMenu selected) {
    // access container at selected position, and show
    menus_[selected_menu_]->interface().show();
}

MenuManager* MenuManager::instance_;

MenuManager& MenuManager::instance() {
    return *instance_;
}

}