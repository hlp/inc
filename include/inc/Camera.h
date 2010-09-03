
#pragma once

#include <cinder/params/Params.h>
#include <cinder/MayaCamUI.h>
#include <cinder/app/App.h>

#include <inc/Module.h>

namespace inc {

    class Camera : public Module, public ci::app::App::Listener {
    public:
        Camera();
        virtual ~Camera();
        
        void setup();
        void update();
        void draw();

        void set_matrices();

        // If a method returns true, event propegation stops. If it returns false,
        // the message propegates to other Listeners.
        virtual bool resize(int, int);
        virtual bool mouseDown(ci::app::MouseEvent);
        virtual bool mouseDrag(ci::app::MouseEvent);
        virtual bool mouseWheel(ci::app::MouseEvent);
        virtual bool keyDown(ci::app::KeyEvent);
        virtual bool keyUp(ci::app::KeyEvent);

        static Camera& instance();

        bool draw_interface();

    private:
        void create_camera();

        ci::MayaCamUI* cam_;
        ci::params::InterfaceGl interface_;
        
        float zoom_speed_;
        float far_clip_;

        bool draw_interface_;

        static Camera* instance_;
    };
}