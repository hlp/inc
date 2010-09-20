
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
    instance_ = this;
}

SolidCreator::~SolidCreator() {
#ifdef TRACE_DTORS
    ci::app::console() << "Deleting SolidCreator" << std::endl;
#endif
}

void SolidCreator::setup() {
    // nothing here
}

void SolidCreator::update() {
    // nothing here
}

void SolidCreator::draw() {
    // Nothing here
}

void SolidCreator::create_rigid_sphere(ci::Vec3f pos, ci::Vec3f radius) {
    SolidPtr ptr = SolidFactory::create_rigid_sphere(pos, radius);

    Manager::instance().solids().push_back(ptr);
    Manager::instance().register_for_selection(ptr);
}

void SolidCreator::create_soft_sphere(ci::Vec3f pos, ci::Vec3f radius) {
    SolidPtr ptr = SolidFactory::create_soft_sphere(pos, radius);

    Manager::instance().solids().push_back(ptr);
    Manager::instance().register_for_selection(ptr);
}

void SolidCreator::create_linked_spheres(ci::Vec3f pos, ci::Vec3f radius) {
    std::tr1::shared_ptr<std::deque<SolidPtr> > d_ptr = 
        SolidFactory::create_linked_soft_spheres(pos, radius);

    std::for_each(d_ptr->begin(), d_ptr->end(),
        [] (SolidPtr s_ptr) { 
            Manager::instance().solids().push_back(s_ptr); 
            Manager::instance().register_for_selection(s_ptr); } );
}

void SolidCreator::create_sphere_matrix(ci::Vec3f pos, ci::Vec3f radius,
    int w, int h, int d) {
    std::tr1::shared_ptr<std::deque<SolidPtr> > d_ptr = 
        SolidFactory::create_soft_sphere_matrix(pos, radius, w, h, d);

    std::for_each(d_ptr->begin(), d_ptr->end(),
        [] (SolidPtr s_ptr) { 
            Manager::instance().solids().push_back(s_ptr); 
            Manager::instance().register_for_selection(s_ptr); } );
}

void SolidCreator::create_rigid_sphere_matrix(ci::Vec3f pos, ci::Vec3f radius,
    int w, int h, int d) {
    std::tr1::shared_ptr<std::deque<SolidPtr> > d_ptr = 
        SolidFactory::create_rigid_sphere_matrix(pos, radius, w, h, d);

    std::for_each(d_ptr->begin(), d_ptr->end(),
        [] (SolidPtr s_ptr) { 
            Manager::instance().solids().push_back(s_ptr);
            Manager::instance().register_for_selection(s_ptr); } );
}

void SolidCreator::create_sphere_spring_matrix(ci::Vec3f pos, ci::Vec3f radius,
    int w, int h, int d) {
    std::tr1::shared_ptr<std::deque<SolidPtr> > d_ptr = 
        SolidFactory::create_rigid_sphere_spring_matrix(pos, radius, w, h, d);

    std::for_each(d_ptr->begin(), d_ptr->end(),
        [] (SolidPtr s_ptr) { 
            Manager::instance().solids().push_back(s_ptr); 
            Manager::instance().register_for_selection(s_ptr); } );
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

SolidCreator* SolidCreator::instance_;

SolidCreator& SolidCreator::instance() {
    return *instance_;
}


}