
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

// GraphicItems do not have positions, rotations, or scales

#include <cinder/Vector.h>
#include <cinder/gl/Vbo.h>

namespace inc {
    class GraphicItem {
    public:
        virtual ~GraphicItem(); 
        virtual void draw() = 0;
    };

    class BoxGraphicItem : public GraphicItem {
    public:
        BoxGraphicItem(ci::Vec3f dimensions);
        virtual ~BoxGraphicItem() { }

        virtual void draw();

    private:
        ci::Vec3f dimensions_;
    };

    class PlaneGraphicItem : public GraphicItem {
    public:
        PlaneGraphicItem(ci::Vec3f dimensions);
        virtual ~PlaneGraphicItem() { }

        virtual void draw();

    private:
        ci::Vec3f dimensions_;
    };

    class OriginGraphicItem : public GraphicItem {
    public:
        OriginGraphicItem();
        virtual ~OriginGraphicItem() { }

        virtual void draw();

        float* grid_plane_size_ptr();
        float* grid_plane_intervals_ptr();

    private:
        void draw_axis();
        void draw_grid_plane_lines();

        float grid_plane_size_;
        float grid_plane_intervals_;
    };

    class VboGraphicItem : public GraphicItem {
    public:
        VboGraphicItem(ci::gl::VboMesh&, ci::Vec3f scale);
        virtual ~VboGraphicItem() { }

        virtual void draw();

    private:
        ci::gl::VboMesh vbo_mesh_;
        ci::Vec3f scale_;
    };
}