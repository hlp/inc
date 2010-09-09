
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

#include <inc/Origin.h>
#include <inc/GraphicItem.h>
#include <inc/Solid.h>

namespace inc {

    Origin::~Origin() {
#ifdef TRACE_DTORS
        ci::app::console() << "Deleting Origin" << std::endl;
#endif

        sock_.reset();
        solid_plane_.reset();
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
        
        solid_plane_ = SolidFactory::create_static_solid_box(
            ci::Vec3f(2000.0f, 10.0f, 2000.0f), 
            ci::Vec3f().zero());
        
        sock_ = SolidFactory::create_sphere_container();
    }

    void Origin::update() {
        // Nothing here
    }

    void Origin::draw() {
        solid_plane_->draw();
        sock_->draw();
        origin_graphic_item_->draw();
    }

}