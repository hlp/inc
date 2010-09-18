
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
#include <cinder/Ray.h>

#include <inc/CurveSketcher.h>

namespace inc {

CurveSketcher::CurveSketcher() {
    instance_ = this;

    active_ = true;
    degree_ = 3;
 
    // TODO: this resolution should change as more points are added
    // in fact, the resolution should represent the number of points
    // between two control points
    rendering_resolution_ = 1.0f / 300.0f;
    line_thickness_ = 3.0f;
    line_color_ = ci::ColorA(1.0f, 0.5f, 0.25f, 0.9f);

    drawing_plane_ = ci::Ray(ci::Vec3f::zero(), ci::Vec3f::yAxis());
}

CurveSketcher::~CurveSketcher() {
    // nothing here
}

void CurveSketcher::setup() {
    // nothing here
}

void CurveSketcher::update() {
    // nothing here
}

void CurveSketcher::draw() {
    if (points_.size() < (size_t)degree_ + 1)
        return;

    ci::gl::color(line_color_);
    glLineWidth(line_thickness_);

    glBegin(GL_LINE_STRIP);

    for (float t = 0.0f; t <= 1.0f; t += rendering_resolution_) {
        ci::gl::vertex(current_spline_->getPosition(t));
    }

    glEnd();
}

void CurveSketcher::on_mouse_click(ci::Ray r) {
    if (!active_)
        return;

    // get the click position from the camera
    // calculate the intersection of the ray with the plane

    // http://local.wasp.uwa.edu.au/~pbourke/geometry/planeline/

    // find distance to plane
    float t = drawing_plane_.getDirection().dot(drawing_plane_.getOrigin() - r.getOrigin()) / 
        drawing_plane_.getDirection().dot(r.getDirection());

    ci::Vec3f p = r.getOrigin() + t * r.getDirection();

    points_.push_back(p);

    generate_spline(false);
}

bool CurveSketcher::activate_button_pressed(bool) {
    if (!active_)
        set_up_sketcher();
    else
        finish_sketcher();

    return false;
}

void CurveSketcher::set_up_sketcher() {
    // create bspline
    current_spline_.reset();
    points_.clear();
}

void CurveSketcher::finish_sketcher() {
    // make a new spline, making sure to close it 
    generate_spline(true);
}

void CurveSketcher::generate_spline(bool close) {
    if (points_.size() < (size_t)degree_)
        return;

    current_spline_ = std::tr1::shared_ptr<ci::BSpline3f>(
        // 3rd param = loop ie should it add extra points to close it
        // 4th param = open ie is it open
        new ci::BSpline3f(points_, degree_, close, !close) );
}

std::tr1::shared_ptr<ci::BSpline3f> CurveSketcher::current_spline() {
    return current_spline_;
}

CurveSketcher* CurveSketcher::instance_;

CurveSketcher& CurveSketcher::instance() {
    return *instance_;
}

}