
#pragma once

#include <cinder/params/Params.h>

#include <inc/Module.h>

namespace inc {

    class SolidCreator : public Module {
    public:
        SolidCreator();
        virtual ~SolidCreator() { }

        void setup();
        void update();
        void draw();

    private:
        void create_object();
        void create_obj();

        ci::params::InterfaceGl interface_;

        bool create_callback_;
        bool last_create_;

        bool create_obj_callback_;
        bool last_create_obj_;
    };
}