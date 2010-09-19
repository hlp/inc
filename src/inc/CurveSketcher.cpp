
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

#include <cinder/Cinder.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/Ray.h>
#include <cinder/cairo/Cairo.h>

#include <inc/CurveSketcher.h>
#include <inc/Camera.h>
#include <incApp.h>

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
    if (points_.size() < (size_t)degree_)
        return;

    ci::gl::color(line_color_);
    glLineWidth(line_thickness_);

    glBegin(GL_LINE_STRIP);

    for (float t = 0.0f; t <= 1.0f; t += rendering_resolution_) {
        ci::gl::vertex(current_spline_->getPosition(t));
    }

    glEnd();

    draw_control_points();
}

void CurveSketcher::draw_control_points() {
    ci::gl::enableDepthWrite(false);
    glEnable(GL_TEXTURE_2D);

    // iterate over all the control points
    std::for_each(control_points_.begin(), control_points_.end(),
        [] (std::tr1::shared_ptr<ControlPoint> point) { point->draw(); } );

    glDisable( GL_TEXTURE_2D );
}

void CurveSketcher::on_mouse_click(ci::Ray r) {
    if (!active_)
        return;

    // check for intersections (selections) of the control points
    for (std::vector<std::tr1::shared_ptr<ControlPoint> >::const_iterator it =
        control_points_.begin(); it != control_points_.end(); ++it) {
        if ((*it)->mouse_pressed(r))
            return;
    }

    // get the click position from the camera
    // calculate the intersection of the ray with the plane

    // http://local.wasp.uwa.edu.au/~pbourke/geometry/planeline/

    // find distance to plane
    float t = drawing_plane_.getDirection().dot(drawing_plane_.getOrigin() - r.getOrigin()) / 
        drawing_plane_.getDirection().dot(r.getDirection());

    ci::Vec3f p = r.getOrigin() + t * r.getDirection();

    points_.push_back(p);

    std::tr1::shared_ptr<ControlPoint> cp = 
        std::tr1::shared_ptr<ControlPoint>(new ControlPoint(p));
    cp->setup();
    control_points_.push_back(cp);

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
    control_points_.clear();
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



ControlPoint::ControlPoint(ci::Vec3f pos) {
    position_ = pos;
    active_ = false;
    arrow_size_ = 0.5f;
    position_image_dim_ = 2.0f;
    position_render_dim_ = position_image_dim_ * 50.0f;
}

bool ControlPoint::mouse_pressed(ci::Ray r) {
    // check if there's been an intersection with the sphere, 
    // if so, activate the object

    // bounding sphere is simply centered on the position,
    // with an arrow_size_ radius
    // copied from Solid::detect_selection

    ci::Vec3f dst = r.getOrigin() - position_;
    float B = dst.dot(r.getDirection());
    float C = dst.dot(dst) - arrow_size_ * arrow_size_;
    float D = B * B - C;

    if (D <= 0) // not an intersection
        return false;

    active_ = true;

    ci::app::console() << "Control Point selected" << std::endl;

    return true;
}

bool ControlPoint::mouse_dragged(ci::app::MouseEvent evt) {
    if (!active_)
        return false;

    //move_point(evt);

    return false;
}

bool ControlPoint::mouse_released(ci::app::MouseEvent evt) {
    active_ = false;

    return false;
}

void ControlPoint::setup() {
	// clear to the background color
    ci::cairo::SurfaceImage base(position_render_dim_, position_render_dim_, true);
	ci::cairo::Context ctx(base);

    // draw a yellow circles
	ctx.setSourceRgb( 1.0f, 1.0f, 0.0f );
	ctx.newSubPath();
    // these coords assume a 8.0 x 8.0 canvas, so scale up as necesarry
    float scl = position_render_dim_ / 8.0f;
    ctx.setLineWidth(1.0f * scl);
    ctx.arc(ci::Vec2f(position_render_dim_ / 2.0f, position_render_dim_ / 2.0f), 
        2.0f * scl, 0, 2 * M_PI);
	ctx.stroke();

    position_image_ = base.getSurface();
    position_texture_ = ci::gl::Texture(position_image_);

    /*
    ci::cairo::SurfaceImage active_base(position_image_dim_, position_image_dim_, true);
	ci::cairo::Context actx(active_base);

    // draw a solid yellow circles
	//actx.setSourceRgb( 0.0f, 1.0f, 0.0f );
	actx.newSubPath();
    //ctx.setLineWidth(1.5f * scl);
    actx.circle(ci::Vec2f(position_image_dim_ / 2.0f, position_image_dim_ / 2.0f), 
        2.0f * scl);
	actx.stroke();

    active_image_ = active_base.getSurface();
    active_texture_ = ci::gl::Texture(active_image_);
    */
}

void ControlPoint::draw() {
    ci::Vec3f cam_right, cam_up;
    Camera::instance().cam().getCamera().getBillboardVectors(&cam_right, &cam_up);

    ci::gl::color(ci::Color::white());

    glBegin(GL_QUADS);

    if (active_) {
        ci::app::console() << "binding active texture" << std::endl;
        active_texture_.bind();
    }
    else
        position_texture_.bind();

    float x = position_.x;
	float y = position_.y;
	float z = position_.z;
			
	float w = position_image_dim_;
	float h = position_image_dim_;

    float perLeft = -1.0f;
	float perRight = 1.0f;
	ci::Vec3f right = cam_right * w;
	ci::Vec3f up = cam_up * h;
			
	glTexCoord2f( 0, 1 );
	glVertex3f(x + right.x * perLeft + up.x * perLeft,
        y + right.y * perLeft + up.y * perLeft,
        z + right.z * perLeft + up.z * perLeft);
	glTexCoord2f( 1, 1 );
	glVertex3f(x + right.x * perRight + up.x * perLeft,
        y + right.y * perRight + up.y * perLeft,
        z + right.z * perRight + up.z * perLeft);
	glTexCoord2f( 1, 0 );
	glVertex3f( x + right.x * perRight + up.x * perRight,
        y + right.y * perRight + up.y * perRight,
        z + right.z * perRight + up.z * perRight);
	glTexCoord2f( 0, 0 );
	glVertex3f( x + right.x * perLeft + up.x * perRight,
        y + right.y * perLeft + up.y * perRight,
        z + right.z * perLeft + up.z * perRight);

    glEnd();
}




}