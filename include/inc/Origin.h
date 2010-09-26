
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

#pragma once

#include <cinder/params/Params.h>

#include <inc/Module.h>

namespace inc {
    class OriginGraphicItem;
    class Solid;

    class Origin : public Module {
    public:
        Origin() { create_ground_plane_ = false; }
        virtual ~Origin();

        void setup();
        void update();
        void draw();

    private:
        OriginGraphicItem* origin_graphic_item_;
        //ci::params::InterfaceGl interface_;

        bool create_ground_plane_;
    };
}