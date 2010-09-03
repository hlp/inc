
#pragma once

// GraphicItems do not have positions, rotations, or scales

#include <cinder/Vector.h>
#include <cinder/gl/Vbo.h>

namespace inc {
    class GraphicItem {
    public:
        virtual ~GraphicItem() { };
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