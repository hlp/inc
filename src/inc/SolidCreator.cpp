
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
        interface_ = ci::params::InterfaceGl("Solid_Creator", ci::Vec2i(100, 100));
        interface_.addParam("Create Object", &create_callback_);
        interface_.addParam("Load Obj", &create_obj_callback_);
    }

    void SolidCreator::update() {
        if (create_callback_ != last_create_) {
            create_soft_sphere(ci::Vec3f(0.0f, 100.0f, 0.0f), ci::Vec3f(15.0f, 15.0f, 15.0f));
            last_create_ = create_callback_;
        } else if (create_obj_callback_ != last_create_obj_) {
            load_obj_as_rigid(ci::Vec3f(0.0f, 100.0f, 0.0f), ci::Vec3f(25.0f, 25.0f, 25.0f));
            last_create_obj_ = create_obj_callback_;
        }
    }

    void SolidCreator::draw() {
        // Nothing here
    }

    void SolidCreator::create_soft_sphere(ci::Vec3f pos, ci::Vec3f size) {
        Manager::instance().solids().push_back(
            SolidFactory::create_soft_sphere(pos, size));
    }

    void SolidCreator::create_solid_box(ci::Vec3f pos, ci::Vec3f size) {
        Manager::instance().solids().push_back(
            SolidFactory::create_solid_box(size,
            pos));
    }

    void SolidCreator::load_obj_as_rigid(ci::Vec3f pos, ci::Vec3f scale) {
        std::string path = ci::app::getOpenFilePath();
        if (path.empty())
            return;

        ci::ObjLoader loader(ci::loadFileStream(path));
        ci::TriMesh mesh;
        loader.load(&mesh, true);

        Manager::instance().solids().push_back(
            SolidFactory::create_rigid_mesh(mesh, pos, scale, 1.0f)); 
    }
}