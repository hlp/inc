
#include <cinder/gl/gl.h>
#include <cinder/app/App.h>

#include <toxi/volume/toxi_volume_VolumetricSpaceVector.h>
#include <toxi/volume/toxi_volume_RoundBrush.h>
#include <toxi/volume/toxi_volume_HashIsoSurface.h>

#include <inc/inc_VolumePainter.h>

namespace inc {

VolumePainter::VolumePainter() {
    brush_radius_ = 50.0f;
    iso_density_ = 1.0f;
    iso_compute_ = 3.0f * 0.001f;
}

void VolumePainter::setup() {
    // nothing here
    volume_ = std::shared_ptr<toxi::volume::VolumetricSpaceVector>(new 
        toxi::volume::VolumetricSpaceVector(ci::Vec3f::one() * 400.0f,
        18.0f, 18.0f, 18.0f));

    brush_ = std::shared_ptr<toxi::volume::RoundBrush>(new
        toxi::volume::RoundBrush(*(volume_.get()), brush_radius_));

    // draw some stuff
    float d = 3.0f;
    brush_->drawAtAbsolutePos(ci::Vec3f::zero(), d);
    brush_->drawAtAbsolutePos(ci::Vec3f(1.0f, 0.0f, 0.0f), d);
    brush_->drawAtAbsolutePos(ci::Vec3f(1.0f, 1.0f, 0.0f), d);
    brush_->drawAtAbsolutePos(ci::Vec3f(1.0f, 0.0f, 1.0f), d);
    brush_->drawAtAbsolutePos(ci::Vec3f(1.0f, 0.0f, 1.0f), d);

    brush_->drawAtGridPos(1.0f, 1.0f, 1.0f, 1.0f);
    brush_->drawAtGridPos(1.0f, 0.0f, 1.0f, 1.0f);


    convert_volume_to_mesh();
}

void VolumePainter::convert_volume_to_mesh() {
    iso_surface_ = std::shared_ptr<toxi::volume::HashIsoSurface>(
        new toxi::volume::HashIsoSurface(*(volume_.get())));

    volume_mesh_ = std::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    iso_surface_->computeSurfaceMesh(volume_mesh_, iso_compute_);

    ci::app::console() << "Mesh triangles = " << volume_mesh_->getNumTriangles() << std::endl;
}

void VolumePainter::update() {
    // nothing here
}

void VolumePainter::draw() {
    if (volume_mesh_.get() == NULL)
        return;

    ci::gl::draw(*(volume_mesh_.get()));
}

}