
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

#include <inc/Menu.h>
#include <incApp.h>
#include <inc/DxfSaver.h>
#include <inc/Manager.h>
#include <inc/Solid.h>

namespace inc {

Menu::Menu() {
    instance_ = this;
}

Menu::~Menu() {
#ifdef TRACE_DTORS
    ci::app::console() << "Deleting Menu" << std::endl;
#endif

    IncApp::instance().unregisterKeyDown(key_down_cb_id_);
}

void Menu::setup() {
    key_down_cb_id_ = IncApp::instance().registerKeyDown(this, &Menu::keyDown);
}

void Menu::update() {
    // nothing here
}

void Menu::draw() {
    // nothing here
}

Menu* Menu::instance_;

Menu& Menu::instance() {
    return *instance_;
}

bool Menu::keyDown(ci::app::KeyEvent k_event) {
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

bool Menu::keyUp(ci::app::KeyEvent k_event) {
    return false;
}

}