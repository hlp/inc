
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

#include <inc/Solid.h>
#include <inc/Renderer.h>
#include <inc/Manager.h>
#include <inc/Camera.h>
#include <inc/Menu.h>
#include <inc/Origin.h>
#include <inc/Module.h>
#include <inc/SolidCreator.h>
#include <inc/CurveSketcher.h>

IncApp::IncApp() {
    instance_ = this;
}

void IncApp::prepareSettings(Settings* settings) {
	settings->setWindowSize(1000, 700);
	settings->setFrameRate(60.0f);
	settings->setResizable(true);
	settings->setFullScreen(false);
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

    main_menu_ = std::tr1::shared_ptr<inc::MainMenu>(new inc::MainMenu());

    origin_ = std::tr1::shared_ptr<inc::Origin>(new inc::Origin());
    manager_->add_module(origin_);

    solid_creator_ = std::tr1::shared_ptr<inc::SolidCreator>(new inc::SolidCreator());
    manager_->add_module(solid_creator_);

    curve_sketcher_ = std::tr1::shared_ptr<inc::CurveSketcher>(new inc::CurveSketcher());
    manager_->add_module(curve_sketcher_);

    // menus must be drawn last
    manager_->add_module(main_menu_);

    manager_->setup_modules();
}

void IncApp::update() {
    manager_->update_modules();
}

void IncApp::draw() {
    renderer_->draw_init(); // Clears the screen

    manager_->draw_modules();

    // Draw the GUI elements of each module
    ci::params::InterfaceGl::draw();
}

void IncApp::shutdown() {
#ifdef TRACE_DTORS
    console() << "IncApp Shutdown" << std::endl;
#endif

    solid_factory_->delete_constraints();

    // delete all objets in the scene
    manager_->clear_solid_list();

    // remove other shared_ptr refs to modules
    manager_->clear_module_list();

    curve_sketcher_.reset();
    solid_creator_.reset();
    origin_.reset();
    main_menu_.reset();
    camera_.reset();
    renderer_.reset();
    solid_factory_.reset();
    manager_.reset();
}

IncApp* IncApp::instance_;

IncApp& IncApp::instance() {
    return *instance_;
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC(IncApp, ci::app::RendererGl)
