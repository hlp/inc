
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

#include <cinder/Camera.h>
#include <cinder/MayaCamUI.h>
#include <cinder/Ray.h>

#include <inc/Camera.h>
#include <incApp.h>
#include <inc/Manager.h>
#include <inc/Solid.h>
#include <inc/CurveSketcher.h>

namespace inc {
    Camera::Camera() {
        instance_ = this;
        
        zoom_speed_ = 30.0f;
        far_clip_ = 10000.0f;
    }

    Camera::~Camera() {
#ifdef TRACE_DTORS
        ci::app::console() << "Deleting Camera" << std::endl;
#endif

        IncApp::instance().unregisterResize(resize_cb_id_);
        IncApp::instance().unregisterMouseDown(mouse_down_cb_id_);
        IncApp::instance().unregisterMouseDrag(mouse_drag_cb_id_);
        IncApp::instance().unregisterMouseWheel(mouse_wheel_cb_id_);

        delete cam_;
    }

    void Camera::setup() {
        resize_cb_id_ = IncApp::instance().registerResize(this, &Camera::resize);
        mouse_down_cb_id_ = IncApp::instance().registerMouseDown(this, &Camera::mouse_down);
        mouse_drag_cb_id_ = IncApp::instance().registerMouseDrag(this, &Camera::mouse_drag);
        mouse_wheel_cb_id_ = IncApp::instance().registerMouseWheel(this, &Camera::mouse_wheel);
        mouse_up_cb_id_ = IncApp::instance().registerMouseUp(this, &Camera::mouse_up);

        cam_ = new ci::MayaCamUI();
        create_camera();

        // set starting position
        ci::CameraPersp new_cam = cam_->getCamera();
        new_cam.setEyePoint(ci::Vec3f(75.0f, 50.0f, 75.0f) * 2.0f);
        new_cam.setCenterOfInterestPoint(ci::Vec3f::zero());
        cam_->setCurrentCam(new_cam);
    }

    void Camera::create_camera() {
        ci::CameraPersp new_cam = cam_->getCamera();
        new_cam.setFarClip(far_clip_);
        new_cam.setAspectRatio(IncApp::instance().getWindowAspectRatio());
        cam_->setCurrentCam(new_cam);
    }

    void Camera::update() {
        set_matrices();
    }

    void Camera::draw() {
        // Nothing here
        //ci::gl::drawLine(ci::Vec3f::zero(), last_test_);
    }

    void Camera::set_matrices() {
        ci::gl::setMatrices(cam_->getCamera());
    }

    bool Camera::resize(ci::app::ResizeEvent) {
        create_camera();
        return false;
    }

    bool Camera::mouse_down(ci::app::MouseEvent m_event) {
        cam_->mouseDown(m_event.getPos());

        was_dragged_ = false;
        down_pos_ = m_event.getPos();

        return false;
    }

    bool Camera::mouse_drag(ci::app::MouseEvent m_event) {
        cam_->mouseDrag(m_event.getPos(), m_event.isLeftDown(), m_event.isMiddleDown(), 
            m_event.isRightDown());

        was_dragged_ = true;

        return false;
    }

    // Zoom code taken from Cinder::MayaCamUI
    bool Camera::mouse_wheel(ci::app::MouseEvent m_event) {
        float mouse_delta = m_event.getWheelIncrement() * zoom_speed_;
        //float direction = mouse_delta > 0.0f ? 1.0 : -1.0;
        
        float newCOI = powf( 2.71828183f, -mouse_delta / 500.0f ) * cam_->getCamera().getCenterOfInterest();
        ci::Vec3f oldTarget = cam_->getCamera().getCenterOfInterestPoint();
        ci::Vec3f newEye = oldTarget - cam_->getCamera().getViewDirection() * newCOI;
        ci::CameraPersp new_cam = cam_->getCamera();
        new_cam.setEyePoint( newEye );
        new_cam.setCenterOfInterest( newCOI );
        cam_->setCurrentCam(new_cam);

        return false;
    }

    bool Camera::mouse_up(ci::app::MouseEvent m_event) {
        // check if it was a click, if so, check selectable for selections
        //if (was_dragged_)
        //    return false;

        if (m_event.getPos().distance(down_pos_) > 2)
            return false;

        check_selection(m_event.getPos());

        return false;
    }

    void Camera::check_selection(ci::Vec2i mouse_pos) {
        SolidList& selectable = Manager::instance().get_selectable();

        ci::Ray mouse_ray = get_ray_from_screen_pos(mouse_pos);

        std::for_each(selectable.begin(), selectable.end(),
            [&] (std::tr1::shared_ptr<Solid> s_ptr) {
            if (s_ptr->detect_selection(mouse_ray)) {
                Manager::instance().deselect_other_solids(s_ptr);
                s_ptr->select();
            }
        } );
    }

    ci::Ray Camera::get_ray_from_screen_pos(ci::Vec2i pos) {
        float w = (float) IncApp::instance().getWindowWidth();
        float h = (float) IncApp::instance().getWindowHeight();

        float u = (float) pos.x / w;
        float v = (float) (h - pos.y) / h;

        return cam_->getCamera().generateRay(u, v, w / h);
    }

    ci::MayaCamUI& Camera::cam() {
        return *cam_;
    }

    Camera* Camera::instance_;

    Camera& Camera::instance() {
        return *instance_;
    }

    bool Camera::draw_interface() {
        return true;
    }
}