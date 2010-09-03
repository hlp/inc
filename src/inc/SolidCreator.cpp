
#include <cinder/params/Params.h>
#include <cinder/app/App.h>
#include <cinder/ObjLoader.h>
#include <cinder/TriMesh.h>

#include <inc/SolidCreator.h>
#include <inc/Solid.h>
#include <inc/Manager.h>

namespace inc {

    SolidCreator::SolidCreator() {
        create_callback_ = false;
        last_create_ = false;
        create_obj_callback_ = false;
        last_create_obj_ = false;
    }

    void SolidCreator::setup() {
        interface_ = ci::params::InterfaceGl("Solid Creator", ci::Vec2i(100, 100));
        interface_.addParam("Create Object", &create_callback_);
        interface_.addParam("Load Obj", &create_obj_callback_);
    }

    void SolidCreator::update() {
        if (create_callback_ != last_create_) {
            create_object();
            last_create_ = create_callback_;
        } else if (create_obj_callback_ != last_create_obj_) {
            create_obj();
            last_create_obj_ = create_obj_callback_;
        }
    }

    void SolidCreator::draw() {
        // Nothing here
    }

    void SolidCreator::create_object() {
        Manager::instance().solids().push_back(
            SolidFactory::create_solid_box(ci::Vec3f(25.0f, 25.0f, 25.0f),
            ci::Vec3f(0.0f, 100.0f, 0.0f)));
    }

    void SolidCreator::create_obj() {
        std::string path = ci::app::getOpenFilePath();
        if (path.empty())
            return;

        ci::ObjLoader loader(ci::loadFileStream(path));
        ci::TriMesh mesh;
        loader.load(&mesh, true);

        for (int i = 0; i < 1; ++i) {
        Manager::instance().solids().push_back(
            SolidFactory::create_soft_body(mesh, ci::Vec3f(0.0, 30.0f*(i+1), 0.0f), 
            ci::Vec3f(10.0f, 10.0f, 10.0f)));
        }
    }
}