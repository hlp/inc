
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

#include <cinder/app/App.h>

#include <inc/inc_Origin.h>
#include <inc/inc_GraphicItem.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Manager.h>

namespace inc {

Origin::Origin() {
    instance_ = this;

    create_ground_plane_ = true;
    draw_grid_ = false;
}

Origin::~Origin() {
#ifdef TRACE_DTORS
    ci::app::console() << "Deleting Origin" << std::endl;
#endif

    delete origin_graphic_item_;
}

void Origin::setup() {
    origin_graphic_item_ = new OriginGraphicItem();

    /*
    interface_ = ci::params::InterfaceGl("Origin", ci::Vec2i(100, 200));
    interface_.addParam("Grid Plane Size", 
        origin_graphic_item_->grid_plane_size_ptr(), "");
    interface_.addParam("Grid Plane Intervals",
        origin_graphic_item_->grid_plane_intervals_ptr(), "");
    */

    create_ground_plane();

    /*
    Manager::instance().solids().push_back(
        SolidFactory::create_soft_sphere_container());
        */
}

void Origin::create_ground_plane() {
    if (!create_ground_plane_)
        return;

    Manager::instance().solids().push_back(
        SolidFactory::create_static_solid_box(
        ci::Vec3f(2000.f, 0.1f, 2000.f), 
        ci::Vec3f().zero()));
}

void Origin::update() {
    // Nothing here
}

void Origin::draw() {
    if (draw_grid_)
        origin_graphic_item_->draw();
}

Origin* Origin::instance_;

}