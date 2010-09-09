
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
#include <incApp.h>

namespace inc {

    class Renderer : public Module {
    public:
        Renderer();
        virtual ~Renderer();
        
        void setup();
        void update();
        void draw();

        void draw_init();

    private:
        void begin3D();
        void draw_objects();
        void end3D();

        ci::ColorA background_color_;

        ci::params::InterfaceGl interface_;
    };

}