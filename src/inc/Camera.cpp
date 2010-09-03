
#include <cinder/Camera.h>
#include <cinder/MayaCamUI.h>

#include <inc/Camera.h>
#include <incApp.h>

namespace inc {
    Camera::Camera() {
        instance_ = this;
        
        zoom_speed_ = 30.0f;
        far_clip_ = 10000.0f;
        draw_interface_ = true;
    }

    Camera::~Camera() {
        delete cam_;
    }

    void Camera::setup() {
        IncApp::instance().addListener(this);

        cam_ = new ci::MayaCamUI();
        create_camera();

        interface_ = ci::params::InterfaceGl("Camera", ci::Vec2i(100, 200));
        interface_.addParam("Zoom Speed", &zoom_speed_, "min=0.1 max=50.0");
        interface_.addParam("Far Clip", &far_clip_, "min=0.1 max=10000000.0");
        
        bool* temp = new bool(false);
        std::string* temp_str = new std::string("foo");

        interface_.addParam("Test Bool", temp, "");
        interface_.addParam("Test string", temp_str, "");
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
        if (draw_interface_)
            return false;

        cam_->mouseDown(m_event.getPos());
        return false;
    }

    bool Camera::mouseDrag(ci::app::MouseEvent m_event) {
        /*
        if (draw_interface_)
            return false;
        */

        cam_->mouseDrag(m_event.getPos(), m_event.isLeftDown(), m_event.isMiddleDown(), 
            m_event.isRightDown());
        return false;
    }

    // Zoom code taken from Cinder::MayaCamUI
    bool Camera::mouseWheel(ci::app::MouseEvent m_event) {
        if (draw_interface_) 
            return false;

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
        return draw_interface_;
    }

    bool Camera::keyDown(ci::app::KeyEvent k_event) {
        // I don't like this hack from a usability standpoint
        /*
        if (k_event.getCode() == ci::app::KeyEvent::KEY_LSHIFT ||
            k_event.getCode() == ci::app::KeyEvent::KEY_RSHIFT)
            draw_interface_ = true; 
        */

        return false;
    }

    bool Camera::keyUp(ci::app::KeyEvent k_event) {
        // removed, see above
        /*
        if (k_event.getCode() == ci::app::KeyEvent::KEY_LSHIFT ||
            k_event.getCode() == ci::app::KeyEvent::KEY_RSHIFT)
            draw_interface_ = false;
        */

        return false;
    }
}