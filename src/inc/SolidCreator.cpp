
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

#include <incApp.h>
#include <inc/SolidCreator.h>
#include <inc/Solid.h>
#include <inc/Manager.h>
#include <inc/CurveSketcher.h> // for ControlPoint
#include <inc/Camera.h>

namespace inc {

SolidCreator::SolidCreator() {
    instance_ = this;

    creation_point_ = std::tr1::shared_ptr<ControlPoint>(
        new ControlPoint(ci::Vec3f::zero(), ci::Color(1.0f, 0.0f, 1.0f)));
}

SolidCreator::~SolidCreator() {
#ifdef TRACE_DTORS
    ci::app::console() << "Deleting SolidCreator" << std::endl;
#endif
}

void SolidCreator::setup() {
    creation_point_->setup();

    mouse_down_cb_id_ = IncApp::instance().registerMouseDown(this, 
        &SolidCreator::mouse_down);
    mouse_drag_cb_id_ = IncApp::instance().registerMouseDrag(this, 
        &SolidCreator::mouse_drag);
    mouse_up_cb_id_ = IncApp::instance().registerMouseUp(this, 
        &SolidCreator::mouse_up);
}

void SolidCreator::update() {
    // nothing here
}

void SolidCreator::draw() {
    creation_point_->draw();
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

ci::Vec3f SolidCreator::creation_point() {
    return creation_point_->position();
}

bool SolidCreator::mouse_down(ci::app::MouseEvent evt) {
    if (!evt.isLeftDown())
        return false;

    ci::Ray r = Camera::instance().get_ray_from_screen_pos(evt.getPos());

    if (creation_point_->mouse_pressed(r))
        creation_point_->set_active(true);

    return false;
}

// TODO: refactor this!! 
// Move get_intersection_with_drawing_plane into somewhere sensible!

bool SolidCreator::mouse_drag(ci::app::MouseEvent evt) {
   if (!evt.isLeftDown())
        return false;

    if (creation_point_->active())
        creation_point_->position() = 
        CurveSketcher::instance().get_intersection_with_drawing_plane(
        Camera::instance().get_ray_from_screen_pos(evt.getPos()));

    return false;
}

bool SolidCreator::mouse_up(ci::app::MouseEvent evt) {
    creation_point_->set_active(false);

    return false;
}

SolidCreator* SolidCreator::instance_;

SolidCreator& SolidCreator::instance() {
    return *instance_;
}


}