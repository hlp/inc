
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

#include <inc/Camera.h>
#include <incApp.h>

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

        IncApp::instance().removeListener(this);

        delete cam_;
    }

    void Camera::setup() {
        IncApp::instance().addListener(this);

        cam_ = new ci::MayaCamUI();
        create_camera();
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
    }

    void Camera::set_matrices() {
        ci::gl::setMatrices(cam_->getCamera());
    }

    bool Camera::resize(int, int) {
        create_camera();
        return false;
    }

    bool Camera::mouseDown(ci::app::MouseEvent m_event) {
        cam_->mouseDown(m_event.getPos());
        return false;
    }

    bool Camera::mouseDrag(ci::app::MouseEvent m_event) {
        cam_->mouseDrag(m_event.getPos(), m_event.isLeftDown(), m_event.isMiddleDown(), 
            m_event.isRightDown());
        return false;
    }

    // Zoom code taken from Cinder::MayaCamUI
    bool Camera::mouseWheel(ci::app::MouseEvent m_event) {
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

    Camera* Camera::instance_;

    Camera& Camera::instance() {
        return *instance_;
    }

    bool Camera::draw_interface() {
        return true;
    }

    /*
    bool Camera::keyDown(ci::app::KeyEvent k_event) {
        return false;
    }

    bool Camera::keyUp(ci::app::KeyEvent k_event) {
        return false;
    }
    */
}