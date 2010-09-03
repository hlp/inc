
#pragma once

#include <cinder/params/Params.h>

#include <inc/Module.h>
#include <incApp.h>

namespace inc {

    class Renderer : public Module {
    public:
        Renderer();
        
        void setup();
        void update();
        void draw();

        void draw_init();

        //ci::params::InterfaceGl& interface();

    private:
        void begin3D();
        void draw_objects();
        void end3D();

        ci::ColorA background_color_;

        ci::params::InterfaceGl interface_;
    };

}