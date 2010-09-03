
#pragma once

#include <cinder/params/Params.h>

namespace inc {

    class Module {
    public:
        virtual ~Module() { }

        virtual void setup() = 0;
        virtual void update() = 0;
        virtual void draw() = 0;

        // Rethink how to require Modules to have GUIs 
        //virtual ci::params::InterfaceGl& interface() = 0;
    };
}