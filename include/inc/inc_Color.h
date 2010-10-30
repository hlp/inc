
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

// needs to keep track of how things are being rendered, either via lighting
// (requiring materials) or vertex coloring

#include <cinder/gl/gl.h>

namespace inc {

class ColorInterface;

// Color is instantiated in inc::Renderer
class Color {
public:
    Color() {
        mode_ = LIGHTING;
        color_interface_ = std::shared_ptr<ColorInterface>(new LightingColor());
    }

    static enum ColorMode {
        FLAT = 0,
        LIGHTING = 1
    };

    static void set_color_mode(ColorMode mode) {
        if (mode_ == mode)
            return;

        mode_ = mode;

        if (mode == FLAT)
            color_interface_ = std::shared_ptr<ColorInterface>(new FlatColor());
        else if (mode == LIGHTING)
            color_interface_ = std::shared_ptr<ColorInterface>(new LightingColor());
    }

    static void set_color(float r, float g, float b) { 
        color_interface_->set_color(r, g, b);
    }

    static void set_color(const ci::Color& c) {
        color_interface_->set_color(c);
    }

    static void set_color_a(float r, float g, float b, float a) {
        color_interface_->set_color_a(r, g, b, a);
    }

    static void set_color_a(const ci::ColorA& c) {
        color_interface_->set_color_a(c);
    }

private:
    static ColorMode mode_;
    static std::shared_ptr<ColorInterface> color_interface_;
};

Color::ColorMode Color::mode_;
std::shared_ptr<ColorInterface> Color::color_interface_;

class ColorInterface {
public:
    virtual ~ColorInterface() { }

    virtual void set_color(GLfloat*) = 0; // expects a buffer with 3 elements
    virtual void set_color(float r, float g, float b) = 0;
    virtual void set_color(const ci::Color&) = 0;
    virtual void set_color_a(GLfloat*) = 0; // expects a buffer with 4 elements
    virtual void set_color_a(float r, float g, float b, float a) = 0;
    virtual void set_color_a(const ci::ColorA&) = 0;
};

class FlatColor : public ColorInterface {
public:
    FlatColor() { }

    virtual void set_color(GLfloat* c) { glColor3f(c[0], c[1], c[2]); }
    virtual void set_color(float r, float g, float b) { glColor3f(r, g, b); }
    virtual void set_color(const ci::Color& c) { glColor3f(c); }
    virtual void set_color_a(GLfloat* c) { glColor4f(c[0], c[1], c[2], c[3]); }
    virtual void set_color_a(float r, float g, float b, float a) { 
        glColor4f(r, g, b, a); 
    }
    virtual void set_color_a(const ci::ColorA& c) { glColor4f(c); }
};

class LightingColor : public ColorInterface {
public:
    LightingColor() { }

    virtual void set_color(GLfloat* c) {
        set_color_a(c[0], c[1], c[2], 1.0f);
    }

    virtual void set_color(float r, float g, float b) {
        set_color_a(r, g, b, 1.0f);
    }

    virtual void set_color(const ci::Color& c) {
        set_color_a(c.r, c.g, c.b, 1.0f);
    }

    virtual void set_color_a(const ci::ColorA& c) { set_color_a(c.r, c.g, c.b, c.a); }   

    virtual void set_color_a(float r, float g, float b, float a) {
        colors[0] = r;
        colors[1] = g;
        colors[2] = b;
        colors[3] = a;

        set_color(&colors[0]);
    }

    virtual void set_color_a(GLfloat* c) { 
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, c);
    }

private:
    GLfloat colors[4];
};

}
