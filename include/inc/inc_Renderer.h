
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

#pragma once

#include <cinder/params/Params.h>
#include <cinder/Color.h>

#include <inc/inc_Module.h>
#include <incApp.h>

namespace inc {

class Light {
public:
    Light(int index) : index_(index) {
        gl_index_ = gl_index();
    }

    virtual void draw() = 0;

    GLenum gl_index();

    void enable() { glEnable(gl_index()); }

    // this is the unique OpenGL light index
    int index_;
    GLenum gl_index_;
};

class CameraLight : public Light {
public:
    CameraLight(int index) : Light(index) { }

    void draw();
};

class Renderer : public Module {
public:
    Renderer();
    virtual ~Renderer();
        
    void setup();
    void update();
    void draw();

    void draw_init();

    ci::ColorA* base_color_ptr() { return &base_color_; }
    ci::ColorA* top_color_ptr() { return &top_color_; }
    ci::ColorA* line_color_ptr() { return &line_color_; }
    float* line_thickness_ptr() { return &line_thickness_; }

    ci::ColorA& base_color() { return base_color_; }
    ci::ColorA& top_color() { return top_color_; }
    ci::ColorA& line_color() { return line_color_; }
    float& line_thickness() { return line_thickness_; }

    ci::ColorA* solids_base_color_ptr() { return &solids_base_color_; }
    ci::ColorA* solids_top_color_ptr() { return &solids_top_color_; }
    ci::ColorA* solids_line_color_ptr() { return &solids_line_color_; }
    float* solids_line_thickness_ptr() { return &solids_line_thickness_; }

    ci::ColorA& solids_base_color() { return solids_base_color_; }
    ci::ColorA& solids_top_color() { return solids_top_color_; }
    ci::ColorA& solids_line_color() { return solids_line_color_; }
    float& solids_line_thickness() { return solids_line_thickness_; }

    bool* enable_alpha_blending_ptr() { return &enable_alpha_blending_; }
    bool* enable_depth_read_ptr() { return &enable_depth_read_; }
    bool* enable_depth_write_ptr() { return &enable_depth_write_; }
    bool* enable_lighting_ptr() { return &enable_lighting_; }
    bool enable_lighting() { return enable_lighting_; }

    static Renderer& instance() { return *instance_; }

    // used for saving a higher resolution image
    void save_image(int width, std::string name);

private:
    void begin3D();
    void draw_objects();
    void end3D();
    void draw_lights();

    std::tr1::shared_ptr<CameraLight> cam_light_;

    ci::ColorA base_color_;
    ci::ColorA top_color_;
    ci::ColorA line_color_;
    float line_thickness_;

    ci::ColorA solids_base_color_;
    ci::ColorA solids_top_color_;
    ci::ColorA solids_line_color_;
    float solids_line_thickness_;

    static Renderer* instance_;

    bool enable_alpha_blending_;
    bool enable_depth_read_;
    bool enable_depth_write_;
    bool enable_lighting_;

    ci::ColorA background_color_;
};



}