
#pragma once

#include <cinder/app/AppBasic.h>

namespace inc {
    class SolidFactory;
    class Renderer;
    class Manager;
    class Camera;
    class Origin;
    class SolidCreator;
}

class IncApp : public ci::app::AppBasic {
    public:
        IncApp();

        void prepareSettings(Settings*);
        void setup();
        void update();
        void draw();
        void shutdown();

        static IncApp& instance();

    private:
        // Access these with T::instance()
        std::tr1::shared_ptr<inc::SolidFactory> solid_factory_;
        std::tr1::shared_ptr<inc::Renderer> renderer_;
        std::tr1::shared_ptr<inc::Manager> manager_;
        std::tr1::shared_ptr<inc::Camera> camera_;
        std::tr1::shared_ptr<inc::Origin> origin_;
        std::tr1::shared_ptr<inc::SolidCreator> solid_creator_;

        static IncApp* instance_;
};