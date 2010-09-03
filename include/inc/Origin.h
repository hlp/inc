
#pragma once

#include <cinder/params/Params.h>

#include <inc/Module.h>

namespace inc {
    class OriginGraphicItem;
    class Solid;

    class Origin : public Module {
    public:
        Origin() { }
        virtual ~Origin();

        void setup();
        void update();
        void draw();

    private:
        OriginGraphicItem* origin_graphic_item_;
        ci::params::InterfaceGl interface_;
        std::tr1::shared_ptr<Solid> solid_plane_;
    };
}