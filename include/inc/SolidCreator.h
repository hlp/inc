
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
        void create_soft_sphere(ci::Vec3f pos, ci::Vec3f size);
        void create_solid_box(ci::Vec3f pos, ci::Vec3f size);
        void load_obj_as_rigid(ci::Vec3f pos, ci::Vec3f scale);

        ci::params::InterfaceGl interface_;

        bool create_callback_;
        bool last_create_;

        bool create_obj_callback_;
        bool last_create_obj_;
    };
}