
#include <inc/Origin.h>
#include <inc/GraphicItem.h>
#include <inc/Solid.h>

namespace inc {

    Origin::~Origin() {
        solid_plane_.reset();
        delete origin_graphic_item_;
    }

    void Origin::setup() {
        origin_graphic_item_ = new OriginGraphicItem();

        interface_ = ci::params::InterfaceGl("Origin", ci::Vec2i(100, 200));
        interface_.addParam("Grid Plane Size", 
            origin_graphic_item_->grid_plane_size_ptr(), "");
        interface_.addParam("Grid Plane Intervals",
            origin_graphic_item_->grid_plane_intervals_ptr(), "");

        solid_plane_ = SolidFactory::create_static_solid_box(
            ci::Vec3f(2000.0f, 10.0f, 2000.0f), 
            ci::Vec3f().zero());
    }

    void Origin::update() {
        // Nothing here
    }

    void Origin::draw() {
        solid_plane_->draw();
        origin_graphic_item_->draw();
    }

}