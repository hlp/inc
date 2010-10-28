
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

#include <incApp.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Renderer.h>
#include <inc/inc_Manager.h>
#include <inc/inc_Camera.h>
#include <inc/inc_Menu.h>
#include <inc/inc_Origin.h>
#include <inc/inc_Module.h>
#include <inc/inc_SolidCreator.h>
#include <inc/inc_CurveSketcher.h>
#include <inc/inc_MeshCreator.h>
#include <inc/inc_LinkFactory.h>
#include <inc/inc_CylinderFactory.h>

IncApp::IncApp() {
    instance_ = this;

    draw_interface_ = true;
}

void IncApp::prepareSettings(Settings* settings) {
	settings->setWindowSize(1000, 700);
	settings->setFrameRate(60.0f);
	settings->setResizable(true);
	settings->setFullScreen(false);
    settings->setTitle("inc");
}

void IncApp::setup() {
    manager_ = std::tr1::shared_ptr<inc::Manager>(new inc::Manager());
    manager_->add_module(manager_);
    
    solid_factory_ = std::tr1::shared_ptr<inc::SolidFactory>(new inc::SolidFactory());
    manager_->add_module(solid_factory_);

    renderer_ = std::tr1::shared_ptr<inc::Renderer>(new inc::Renderer());
    manager_->add_module(renderer_);
    
    camera_ = std::tr1::shared_ptr<inc::Camera>(new inc::Camera());
    manager_->add_module(camera_);

    menu_manager_ = std::tr1::shared_ptr<inc::MenuManager>(new inc::MenuManager());
    manager_->add_module(menu_manager_);

    origin_ = std::tr1::shared_ptr<inc::Origin>(new inc::Origin());
    manager_->add_module(origin_);

    solid_creator_ = std::tr1::shared_ptr<inc::SolidCreator>(new inc::SolidCreator());
    manager_->add_module(solid_creator_);

    curve_sketcher_ = std::tr1::shared_ptr<inc::CurveSketcher>(new inc::CurveSketcher());
    manager_->add_module(curve_sketcher_);

    // other object
    mesh_creator_ = std::tr1::shared_ptr<inc::MeshCreator>(new inc::MeshCreator());
    mesh_creator_->setup();

    link_factory_ = std::tr1::shared_ptr<inc::LinkFactory>(new inc::LinkFactory());
    link_factory_->setup();

    manager_->setup_modules();

    
}

void IncApp::update() {
    manager_->update_modules();
}

void IncApp::draw() {
    renderer_->draw_init(); // Clears the screen

    manager_->draw_modules();

    inc::MeshCreator::instance().draw();
    
    if (draw_interface_) {
        ci::params::InterfaceGl::draw();
        menu_manager_->delayed_draw();
    }
}

void IncApp::shutdown() {
#ifdef TRACE_DTORS
    console() << "IncApp Shutdown" << std::endl;
#endif

    mesh_creator_.reset();

    // delete all objets in the scene
    manager_->clear_graphic_item_list();
    manager_->clear_solid_list();

    // NOTE: Solids and GraphicItems will try to clean up all code associated
    // with Bullet, deleting bodies and constraints. This code goes in and deletes 
    // any objects that are left. That said, this must be calle after all the other
    // objects have been deleted, therwise they will try deleting objects in Bullet
    // that have already been deleted (a very bad thing indeed).
    solid_factory_->delete_constraints();

    // remove other shared_ptr refs to modules
    manager_->clear_module_list();

    menu_manager_.reset();
    curve_sketcher_.reset();
    solid_creator_.reset();
    origin_.reset();
    force_menu_.reset();
    camera_.reset();
    renderer_.reset();
    solid_factory_.reset();
    manager_.reset();
}

void IncApp::set_draw_interface(bool di) {
    draw_interface_ = di;
}

IncApp* IncApp::instance_;

IncApp& IncApp::instance() {
    return *instance_;
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC(IncApp, ci::app::RendererGl)
