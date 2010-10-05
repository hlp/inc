
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
#include <inc/MeshCreator.h>

namespace inc {

CurveSketcher::CurveSketcher() {
    instance_ = this;

    active_ = false;
    degree_ = 3;
 
    // TODO: this resolution should change as more points are added
    // in fact, the resolution should represent the number of points
    // between two control points
    rendering_resolution_ = 1.0f / 300.0f;
    line_thickness_ = 3.0f;
    line_color_ = ci::ColorA(1.0f, 0.5f, 0.25f, 0.9f);

    drawing_plane_ = ci::Ray(ci::Vec3f(0.0f, 0.0f, 0.0f), ci::Vec3f::yAxis());

    refining_curve_ = false;
}

CurveSketcher::~CurveSketcher() {
    IncApp::instance().unregisterMouseDown(mouse_down_cb_id_);
    IncApp::instance().unregisterMouseDrag(mouse_drag_cb_id_);
    IncApp::instance().unregisterMouseUp(mouse_up_cb_id_);

    active_point_.reset();
    current_spline_.reset();
    control_points_.clear();
}

void CurveSketcher::setup() {
    mouse_down_cb_id_ = IncApp::instance().registerMouseDown(this, &CurveSketcher::mouse_down);
    mouse_drag_cb_id_ = IncApp::instance().registerMouseDrag(this, &CurveSketcher::mouse_drag);
    mouse_up_cb_id_ = IncApp::instance().registerMouseUp(this, &CurveSketcher::mouse_up);
}

void CurveSketcher::update() {
    // nothing here
}

void CurveSketcher::draw() {
    // I don't know why this is needed, but it freaks out 
    // when control_points_ has size zero
    if (control_points_.empty())
        return;    

    if ((control_points_.size() - 1) > (size_t)degree_) {
        // draw the b spline
        ci::gl::color(line_color_);
        glLineWidth(line_thickness_);

        glBegin(GL_LINE_STRIP);

        for (float t = 0.0f; t <= 1.0f; t += rendering_resolution_) {
            ci::gl::vertex(current_spline_->getPosition(t));
        }

        glEnd();
    }

    draw_control_points();
}

void CurveSketcher::draw_control_points() {
    ci::gl::enableDepthWrite(false);

    // iterate over all the control points
    std::for_each(control_points_.begin(), control_points_.end(),
        [] (std::tr1::shared_ptr<ControlPoint>& point) { point->draw(); } );
}

bool* CurveSketcher::active_ptr() {
    return &active_;
}

bool CurveSketcher::activate_button_pressed(bool) {
    if (active_) // bool was set by menu
        set_up_sketcher();
    else
        finish_sketcher();

    return false;
}

void CurveSketcher::set_up_sketcher() {
    // create bspline
    current_spline_.reset();
    control_points_.clear();

    refining_curve_ = false;
}

void CurveSketcher::finish_sketcher() {
    if (invalid_curve())
        return;

    // make a new spline, making sure to close it 
    generate_spline(true);

    // generate a mesh now
    MeshCreator::instance().add_bspline_mesh(current_spline_);

    refining_curve_ = true;
}

bool CurveSketcher::invalid_curve() {
    // I don't know why this is needed, but it freaks out 
    // when control_points_ has size zero
    if (control_points_.empty())
        return true;

    if ((control_points_.size() - 1) < (size_t)degree_)
        return true;

    return false;
}

void CurveSketcher::generate_spline(bool close) {
    if (invalid_curve())
        return;

    std::vector<ci::Vec3f> points;

    std::for_each(control_points_.begin(), control_points_.end(),
        [&] (std::tr1::shared_ptr<ControlPoint> ptr) {
            points.push_back(ptr->position());
    } );

    current_spline_ = std::tr1::shared_ptr<ci::BSpline3f>(
        // 3rd param = loop ie should it add extra points to close it
        // 4th param = open ie is it open
        new ci::BSpline3f(points, degree_, close, !close) );
}

std::tr1::shared_ptr<ci::BSpline3f> CurveSketcher::current_spline() {
    return current_spline_;
}

// this is used for the starting position for soft body creation
ci::Vec3f CurveSketcher::current_spline_center() {
    if (invalid_curve())
        return ci::Vec3f::zero();

    int count = 0;
    ci::Vec3f sum = ci::Vec3f::zero();

    for (float t = 0.0f; t <= 1.0f; t += 0.01) {
        sum += current_spline_->getPosition(t);
        ++count;
    }

    return sum / count;
}

// on first press check for intersection, if so, do nothing and wait for a drag
// if no intersection, make a new control point, deactivate any other active
// control points, and wait for a drag

bool CurveSketcher::mouse_down(ci::app::MouseEvent evt) {
    if (!refining_curve_ && !active_)
        return false;

    if (!evt.isLeftDown())
        return false;

    ci::Ray r = Camera::instance().get_ray_from_screen_pos(evt.getPos());

    std::tr1::shared_ptr<ControlPoint> selected;

    if (check_control_point_intersection(r, selected)) {
        selected->set_active(true); // make sure selected is active
        active_point_ = selected;
        deactivate_all_but_active();
        return false; // wait for drag
    }

    if (refining_curve_)
        return false;

    create_new_control_point(r);
    deactivate_all_but_active();

    generate_spline(false);

    return false;
}

bool CurveSketcher::check_control_point_intersection(ci::Ray r,
    std::tr1::shared_ptr<ControlPoint>& selected) {
    // check for intersections (selections) of the control points
    for (std::vector<std::tr1::shared_ptr<ControlPoint> >::const_iterator it =
        control_points_.begin(); it != control_points_.end(); ++it) {
        if ((*it)->mouse_pressed(r)) {
            selected = *it;
            return true;
        }
    }

    selected.reset(); // make sure this is null
    return false;
}

void CurveSketcher::create_new_control_point(ci::Ray r) {
    std::tr1::shared_ptr<ControlPoint> cp = 
        std::tr1::shared_ptr<ControlPoint>(
        new ControlPoint(get_intersection_with_drawing_plane(r)));
    cp->setup();
    control_points_.push_back(cp);

    active_point_ = cp;
}

ci::Vec3f CurveSketcher::get_intersection_with_drawing_plane(ci::Ray r) {
    // based on:
    // http://local.wasp.uwa.edu.au/~pbourke/geometry/planeline/

    // find distance to plane
    float t = drawing_plane_.getDirection().dot(drawing_plane_.getOrigin() - r.getOrigin()) / 
        drawing_plane_.getDirection().dot(r.getDirection());

    return r.getOrigin() + t * r.getDirection();
}

void CurveSketcher::deactivate_all_but_active() {
    for (std::vector<std::tr1::shared_ptr<ControlPoint> >::const_iterator it =
        control_points_.begin(); it != control_points_.end(); ++it) {
        if (*it == active_point_)
            continue;

        (*it)->set_active(false);
    }
}

bool CurveSketcher::mouse_drag(ci::app::MouseEvent evt) {
    if (!refining_curve_ && !active_)
        return false;

    if (!evt.isLeftDown())
        return false;

    active_point_->position() = get_intersection_with_drawing_plane(
        Camera::instance().get_ray_from_screen_pos(evt.getPos()));

    if (!refining_curve_)
        generate_spline(false);
    else {
        generate_spline(true);
        // regenerate mesh
        MeshCreator::instance().rebuild_mesh();
    }

    return false;
}

bool CurveSketcher::mouse_up(ci::app::MouseEvent evt) {
    std::for_each(control_points_.begin(), control_points_.end(),
        [=] (std::tr1::shared_ptr<ControlPoint> cp) { cp->set_active(false); } );

    return false;
}



CurveSketcher* CurveSketcher::instance_;

CurveSketcher& CurveSketcher::instance() {
    return *instance_;
}




ControlPoint::ControlPoint(ci::Vec3f pos, ci::Color col)
    : position_(pos), color_(col) {
    active_ = false;
    arrow_size_ = 0.5f;

    position_image_dim_ = 0.5f;
    position_render_dim_ = position_image_dim_ * 50.0f;
    arrow_size_ = position_image_dim_;

    arrow_color_ = ci::ColorA(1.0f, 1.0f, 0.0f, 1.0f);
    arrow_line_width_ = 1.25f * arrow_size_;
    arrow_base_length_ = 1.5f * arrow_size_;
    arrow_triangle_length_ = 2.0f * arrow_size_;
    arrow_triangle_height_ = 1.75f * arrow_size_;
}

ControlPoint::~ControlPoint() {
    // nothing here
}

bool ControlPoint::mouse_pressed(ci::Ray r) {
    // check if there's been an intersection with the sphere, 
    // if so, activate the object

    // bounding sphere is simply centered on the position,
    // with an arrow_size_ radius
    // copied from Solid::detect_selection

    ci::Vec3f dst = r.getOrigin() - position_;
    float B = dst.dot(r.getDirection());
    float C = dst.dot(dst) - position_image_dim_ * position_image_dim_;
    float D = B * B - C;

    if (D <= 0) // not an intersection
        return false;

    return true;
}

bool ControlPoint::active() {
    return active_;
}

void ControlPoint::set_active(bool a) {
    active_ = a;
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

////////////////////////
// WARNING !! /////////
///////////////////////
// This method generates a memory leak!
// I don't know how to fix it
void ControlPoint::setup() {

    /*
	// clear to the background color
    ci::cairo::SurfaceImage base(position_render_dim_, position_render_dim_, true);
	ci::cairo::Context ctx(base);
    
    // draw a light gray circle
    ctx.setSourceRgba(1.0f, 1.0f, 1.0f, 0.75f);
	ctx.newSubPath();
    // these coords assume a 8.0 x 8.0 canvas, so scale up as necesarry
    float scl = position_render_dim_ / 8.0f;
    ctx.setLineWidth(1.0f * scl);
    ctx.circle(ci::Vec2f(position_render_dim_ / 2.0f, position_render_dim_ / 2.0f), 2.0f * scl);
	ctx.stroke();

    position_image_ = base.getSurface();
    position_texture_ = ci::gl::Texture(position_image_);

    base = ci::cairo::SurfaceImage(position_render_dim_, position_render_dim_, true);
    ctx = ci::cairo::Context(base);

    ctx.setSourceRgb( 1.0f, 1.0f, 0.0f );
    ctx.newSubPath();
    ctx.setLineWidth(1.0f * scl);
    ctx.circle(ci::Vec2f(position_render_dim_ / 2.0f, position_render_dim_ / 2.0f), 
        2.0f * scl);
	ctx.stroke();

    active_image_ = base.getSurface();
    active_texture_ = ci::gl::Texture(active_image_);

    */

    set_active(false);
}

void ControlPoint::draw() {
    if (active_)
        draw_arrows();

    ci::gl::color(color_);

    ci::gl::drawCube(position_, ci::Vec3f::one() * position_image_dim_);

    /*

    glEnable(GL_TEXTURE_2D);

    ci::Vec3f cam_right, cam_up;
    Camera::instance().cam().getCamera().getBillboardVectors(&cam_right, &cam_up);

    if (active_)
        active_texture_.bind();
    else
        position_texture_.bind();

    glBegin(GL_QUADS);

    float x = position_.x;
	float y = position_.y;
	float z = position_.z;
			
	float w = position_image_dim_;
	float h = position_image_dim_;

    float perLeft = -1.0f;
	float perRight = 1.0f;
	ci::Vec3f right = cam_right * w;
	ci::Vec3f up = cam_up * h;
			
	glTexCoord2f(0, 1);
	glVertex3f(x + right.x * perLeft + up.x * perLeft,
        y + right.y * perLeft + up.y * perLeft,
        z + right.z * perLeft + up.z * perLeft);
	glTexCoord2f(1, 1);
	glVertex3f(x + right.x * perRight + up.x * perLeft,
        y + right.y * perRight + up.y * perLeft,
        z + right.z * perRight + up.z * perLeft);
	glTexCoord2f(1, 0);
	glVertex3f( x + right.x * perRight + up.x * perRight,
        y + right.y * perRight + up.y * perRight,
        z + right.z * perRight + up.z * perRight);
	glTexCoord2f(0, 0);
	glVertex3f( x + right.x * perLeft + up.x * perRight,
        y + right.y * perLeft + up.y * perRight,
        z + right.z * perLeft + up.z * perRight);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    */
}

void ControlPoint::draw_arrows() {
    ci::gl::color(arrow_color_);
    glLineWidth(arrow_line_width_);

    glBegin(GL_LINES);

    // X INCREASE
    ci::gl::vertex(position_ + ci::Vec3f(position_image_dim_, 0.0f, 0.0f));
    ci::gl::vertex(position_ + ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        0.0f, 0.0f));

    ci::gl::vertex(position_ + ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        -arrow_triangle_height_ / 2.0f, 0.0f));
    ci::gl::vertex(position_ + ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        arrow_triangle_height_ / 2.0f, 0.0f));
    
    ci::gl::vertex(position_ + ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        arrow_triangle_height_ / 2.0f, 0.0f));
    ci::gl::vertex(position_ + ci::Vec3f(arrow_base_length_ + position_image_dim_ + arrow_triangle_length_, 
        0.0f, 0.0f));

    ci::gl::vertex(position_ + ci::Vec3f(arrow_base_length_ + position_image_dim_ + arrow_triangle_length_, 
        0.0f, 0.0f));
    ci::gl::vertex(position_ + ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        -arrow_triangle_height_ / 2.0f, 0.0f));


    // X DECREASE
    ci::gl::vertex(position_ - ci::Vec3f(position_image_dim_, 0.0f, 0.0f));
    ci::gl::vertex(position_ - ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        0.0f, 0.0f));

    ci::gl::vertex(position_ - ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        -arrow_triangle_height_ / 2.0f, 0.0f));
    ci::gl::vertex(position_ - ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        arrow_triangle_height_ / 2.0f, 0.0f));
    
    ci::gl::vertex(position_ - ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        arrow_triangle_height_ / 2.0f, 0.0f));
    ci::gl::vertex(position_ - ci::Vec3f(arrow_base_length_ + position_image_dim_ + arrow_triangle_length_, 
        0.0f, 0.0f));

    ci::gl::vertex(position_ - ci::Vec3f(arrow_base_length_ + position_image_dim_ + arrow_triangle_length_, 
        0.0f, 0.0f));
    ci::gl::vertex(position_ - ci::Vec3f(arrow_base_length_ + position_image_dim_, 
        -arrow_triangle_height_ / 2.0f, 0.0f));


    // Z INCREASE
    ci::gl::vertex(position_ + ci::Vec3f(0.0f, 0.0f, position_image_dim_));
    ci::gl::vertex(position_ + ci::Vec3f(0.0f, 0.0f, arrow_base_length_ + position_image_dim_));

    ci::gl::vertex(position_ + ci::Vec3f(0.0f,
        -arrow_triangle_height_ / 2.0f, arrow_base_length_ + position_image_dim_));
    ci::gl::vertex(position_ + ci::Vec3f(0.0f, 
        arrow_triangle_height_ / 2.0f, arrow_base_length_ + position_image_dim_));
    
    ci::gl::vertex(position_ + ci::Vec3f(0.0f,
        arrow_triangle_height_ / 2.0f, arrow_base_length_ + position_image_dim_));
    ci::gl::vertex(position_ + ci::Vec3f(0.0f, 
        0.0f, arrow_base_length_ + position_image_dim_ + arrow_triangle_length_));

    ci::gl::vertex(position_ + ci::Vec3f(0.0f, 
        0.0f, arrow_base_length_ + position_image_dim_ + arrow_triangle_length_));
    ci::gl::vertex(position_ + ci::Vec3f(0.0f, 
        -arrow_triangle_height_ / 2.0f, arrow_base_length_ + position_image_dim_));


    // Z DECREASE
    ci::gl::vertex(position_ - ci::Vec3f(0.0f, 0.0f, position_image_dim_));
    ci::gl::vertex(position_ - ci::Vec3f(0.0f, 0.0f, arrow_base_length_ + position_image_dim_));

    ci::gl::vertex(position_ - ci::Vec3f(0.0f,
        -arrow_triangle_height_ / 2.0f, arrow_base_length_ + position_image_dim_));
    ci::gl::vertex(position_ - ci::Vec3f(0.0f, 
        arrow_triangle_height_ / 2.0f, arrow_base_length_ + position_image_dim_));
    
    ci::gl::vertex(position_ - ci::Vec3f(0.0f,
        arrow_triangle_height_ / 2.0f, arrow_base_length_ + position_image_dim_));
    ci::gl::vertex(position_ - ci::Vec3f(0.0f, 
        0.0f, arrow_base_length_ + position_image_dim_ + arrow_triangle_length_));

    ci::gl::vertex(position_ - ci::Vec3f(0.0f, 
        0.0f, arrow_base_length_ + position_image_dim_ + arrow_triangle_length_));
    ci::gl::vertex(position_ - ci::Vec3f(0.0f, 
        -arrow_triangle_height_ / 2.0f, arrow_base_length_ + position_image_dim_));


    glEnd();
}

ci::Vec3f& ControlPoint::position() {
    return position_;
}




}