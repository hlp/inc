
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
        for (int i = 0; i < 5; ++i) {
            cb_[i] = false;
            last_cb_[i] = false;
        }

        r_matrix_w_ = 2;
        r_matrix_h_ = 2;
        r_matrix_d_ = 2;

    }

    SolidCreator::~SolidCreator() {
#ifdef TRACE_DTORS
        ci::app::console() << "Deleting SolidCreator" << std::endl;
#endif
    }

    void SolidCreator::setup() {
        interface_ = ci::params::InterfaceGl("Solid_Creator", ci::Vec2i(300, 200));
        interface_.addParam("Create rigid sphere", &cb_[0]);
        interface_.addParam("Create soft sphere", &cb_[1]);
        interface_.addParam("Create linked spheres", &cb_[2]);
        interface_.addParam("Create soft sphere matrix", &cb_[3]);
        interface_.addParam("Create spring matrix", &cb_[4]);
        interface_.addParam("Matrix width", &r_matrix_w_);
        interface_.addParam("Matrix height", &r_matrix_h_);
        interface_.addParam("Matrix depth", &r_matrix_d_);

    }

    void SolidCreator::update() {
        if  (cb_[0] != last_cb_[0]) {
            create_rigid_sphere(ci::Vec3f(0.0f, 250.0f, 0.0f), ci::Vec3f::one() * 3.0f);
            last_cb_[0] = cb_[0];
        } else if (cb_[1] != last_cb_[1]) {
            create_soft_sphere(ci::Vec3f(0.0f, 250.0f, 0.0f), ci::Vec3f::one() * 3.0f);
            last_cb_[1] = cb_[1];
        } else if (cb_[2] != last_cb_[2]) {
            create_linked_spheres(ci::Vec3f(0.0f, 150.0f, 0.0f), ci::Vec3f::one() * 3.0f);
            last_cb_[2] = cb_[2];
        } else if (cb_[3] != last_cb_[3]) {
            // create matrix
            create_sphere_matrix(ci::Vec3f(20.0f, 150.0f, 0.0f), ci::Vec3f::one() * 3.0f,
                4, 4, 4);
            last_cb_[3] = cb_[3];
        } else if (cb_[4] != last_cb_[4]) {
            create_sphere_spring_matrix(ci::Vec3f(0.0f, 150.0f, 0.0f), 
                ci::Vec3f::one() * 3.0f,
                r_matrix_w_, r_matrix_h_, r_matrix_d_);
            last_cb_[4] = cb_[4];
        }
    }

    void SolidCreator::draw() {
        // Nothing here
    }

    void SolidCreator::create_rigid_sphere(ci::Vec3f pos, ci::Vec3f radius) {
        Manager::instance().solids().push_back(
            SolidFactory::create_rigid_sphere(pos, radius));
    }

    void SolidCreator::create_soft_sphere(ci::Vec3f pos, ci::Vec3f radius) {
        Manager::instance().solids().push_back(
            SolidFactory::create_soft_sphere(pos, radius));
    }

    void SolidCreator::create_linked_spheres(ci::Vec3f pos, ci::Vec3f radius) {
        std::tr1::shared_ptr<std::deque<SolidPtr> > d_ptr = 
            SolidFactory::create_linked_soft_spheres(pos, radius);

        std::for_each(d_ptr->begin(), d_ptr->end(),
            [] (SolidPtr s_ptr) { Manager::instance().solids().push_back(s_ptr); } );
    }

    void SolidCreator::create_sphere_matrix(ci::Vec3f pos, ci::Vec3f radius,
        int w, int h, int d) {
        std::tr1::shared_ptr<std::deque<SolidPtr> > d_ptr = 
            SolidFactory::create_soft_sphere_matrix(pos, radius, w, h, d);

        std::for_each(d_ptr->begin(), d_ptr->end(),
            [] (SolidPtr s_ptr) { Manager::instance().solids().push_back(s_ptr); } );
    }

    void SolidCreator::create_sphere_spring_matrix(ci::Vec3f pos, ci::Vec3f radius,
        int w, int h, int d) {
        std::tr1::shared_ptr<std::deque<SolidPtr> > d_ptr = 
            SolidFactory::create_rigid_sphere_matrix(pos, radius, w, h, d);

        std::for_each(d_ptr->begin(), d_ptr->end(),
            [] (SolidPtr s_ptr) { Manager::instance().solids().push_back(s_ptr); } );
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