
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
#include <cinder/app/App.h>

#include <inc/inc_Color.h>

namespace inc {

GLfloat LightingColor::colors_[4];
float LightingColor::shinyness_;
ci::ColorA LightingColor::specular_;

Color::ColorMode Color::mode_ = Color::ColorMode::LIGHTING;
std::shared_ptr<ColorInterface> Color::color_interface_;

void Color::set_color_mode(ColorMode mode) {
    if (mode_ == mode)
        return;

    mode_ = mode;

    if (mode == FLAT)
        color_interface_ = std::shared_ptr<ColorInterface>(new FlatColor());
    else if (mode == LIGHTING)
        color_interface_ = std::shared_ptr<ColorInterface>(new LightingColor());
}

void Color::set_color(float r, float g, float b) { 
    color_interface_->set_color(r, g, b);
}

void Color::set_color(const ci::Color& c) {
    color_interface_->set_color(c);
}

void Color::set_color_a(float r, float g, float b, float a) {
    color_interface_->set_color_a(r, g, b, a);
}

void Color::set_color_a(const ci::ColorA& c) {
    color_interface_->set_color_a(c);
}

}
