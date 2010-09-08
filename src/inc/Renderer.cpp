
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
#include <cinder/params/Params.h>

#include <inc/Renderer.h>
#include <inc/Manager.h>
#include <inc/Solid.h>

namespace inc {

    Renderer::Renderer() {
        background_color_ = ci::ColorA(0.1f, 0.1f, 0.1f, 1.0f);
    }

    void Renderer::setup() {
        interface_ = ci::params::InterfaceGl("Renderer", ci::Vec2i(100, 200));
        interface_.addParam("Background Color", &background_color_, "");
    }

    void Renderer::update() {
        // Nothing here
    }

    void Renderer::draw_init() {
        glClearColor(background_color_.r, background_color_.g,
            background_color_.b, background_color_.a);
	
        ci::gl::enableAlphaBlending();
        ci::gl::enableDepthRead(true);
        ci::gl::enableDepthWrite(true);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    void Renderer::draw() {
        begin3D();
        draw_objects();
        end3D();
    }

    void Renderer::begin3D() {
        // Nothing here
    }

    void Renderer::draw_objects() {
        // Draw Solids
        SolidList& solids = Manager::instance().solids();
        for (SolidList::const_iterator it = solids.begin(); it != solids.end(); ++it) {
            (*it)->draw();
        }

        // Draw GraphicItems
        GraphicItemList& graphic_items = Manager::instance().graphic_items();
        for (GraphicItemList::const_iterator it = graphic_items.begin(); 
            it != graphic_items.end(); ++it) {
            (*it)->draw();
        }
    }

    void Renderer::end3D() {
        // Nothing here
    }
}