
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
#include <cinder/gl/TileRender.h>
#include <cinder/ImageIo.h>

#include <inc/inc_Renderer.h>
#include <inc/inc_Manager.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Camera.h>
#include <inc/inc_SolidCreator.h>

namespace inc {

Renderer::Renderer() {
    instance_ = this;

    enable_alpha_blending_ = true;
    enable_depth_read_ = true;
    enable_depth_write_ = true;
    // NOTE: make sure these two items are in sync
    enable_lighting_ = true;
    Color::use_lighting(true);

    background_color_ = ci::ColorA(0.1f, 0.1f, 0.1f, 1.0f);

    base_color_ = ci::ColorA(1.0f, 0.0, 1.0f, 0.85f);
    top_color_ = ci::ColorA(1.0f, 1.0f, 0.0f, 0.85f);
    line_color_ = ci::ColorA(0.0f, 0.0f, 0.0f, 0.4f);
    line_thickness_ = 0.15f;

    solids_base_color_ = ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f);
    solids_top_color_ = ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f);
    solids_line_color_ = ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f);
    solids_line_thickness_ = 1.0f;
}

Renderer::~Renderer() {
#ifdef TRACE_DTORS
    ci::app::console() << "Deleting Renderer" << std::endl;
#endif

    cam_light_.reset();
}

void Renderer::setup() {
    cam_light_ = std::tr1::shared_ptr<CameraLight>(new CameraLight(0));
}

void Renderer::update() {
    // nothing here
}

void Renderer::draw_init() {
    glClearColor(background_color_.r, background_color_.g,
        background_color_.b, background_color_.a);
	
    if (enable_alpha_blending_)
        ci::gl::enableAlphaBlending();
    ci::gl::enableDepthRead(enable_depth_read_);
    ci::gl::enableDepthWrite(enable_depth_write_);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if (enable_lighting_)
        draw_lights();
}

void Renderer::draw() {
    begin3D();
    draw_objects();
    end3D();
}

void Renderer::begin3D() {
    // nothing here
}

void Renderer::draw_lights() {
    glEnable(GL_LIGHTING);

    cam_light_->enable();
    cam_light_->draw();
}

void Renderer::draw_objects() {
    // Draw Solids
    SolidList& solids = Manager::instance().solids();
    for (SolidList::const_iterator it = solids.begin(); it != solids.end(); ++it) {
        if ((*it)->visible())
            (*it)->draw();
    }

    // Draw GraphicItems
    GraphicItemList& graphic_items = Manager::instance().graphic_items();
    for (GraphicItemList::const_iterator it = graphic_items.begin(); 
        it != graphic_items.end(); ++it) {
        if ((*it)->visible())
            (*it)->draw();
    }
}

void Renderer::end3D() {
    if (enable_lighting_)
        glDisable(GL_LIGHTING);
}

Renderer* Renderer::instance_;

void Renderer::save_image(int width, std::string name) {
    // changing the render matrix (ie drawing the interface) messes up the 
    // tile renderer
    IncApp::instance().set_draw_interface(false);

    ci::gl::TileRender tr(width, 
        (float)width / (float)(IncApp::instance().getWindowWidth()) * 
        (float)(IncApp::instance().getWindowHeight()));

	tr.setMatrices(Camera::instance().cam().getCamera());

    while(tr.nextTile()) {
        IncApp::instance().draw();
    }

    ci::writeImage(name, tr.getSurface());

    IncApp::instance().set_draw_interface(true);
}

GLenum Light::gl_index() {
    switch (index_) {
        case 0:
        default:
            return GL_LIGHT0;
    }

    return GL_LIGHT0;
}

void CameraLight::draw() {
    // set up a directional (sun-like) light far away from the model
	GLfloat light_position[] = { 50.0f, 1.0f, 50.0f, 0.0f };
    GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
    
	glLightfv(gl_index_, GL_POSITION, light_position);
    glLightfv(gl_index_, GL_DIFFUSE, white_light);
    glLightfv(gl_index_, GL_SPECULAR, white_light);
}

}