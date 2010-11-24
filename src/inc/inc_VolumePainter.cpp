
#include <cinder/gl/gl.h>

#include <toxi/volume/toxi_volume_VolumetricSpaceVector.h>
#include <toxi/volume/toxi_volume_RoundBrush.h>
#include <toxi/volume/toxi_volume_HashIsoSurface.h>

#include <inc/inc_VolumePainter.h>

namespace inc {

VolumePainter::VolumePainter() {
    brush_radius_ = 5.0f;
    iso_density_ = 1.0f;
    iso_compute_ = 1.0f;
}

void VolumePainter::setup() {
    // nothing here
    volume_ = std::shared_ptr<toxi::volume::VolumetricSpaceVector>(new 
        toxi::volume::VolumetricSpaceVector(ci::Vec3f::one() * 10.0f,
        100.0f, 100.0f, 100.0f));

    brush_ = std::shared_ptr<toxi::volume::RoundBrush>(new
        toxi::volume::RoundBrush(*(volume_.get()), brush_radius_));

    // draw some stuff

    convert_volume_to_mesh();
}

void VolumePainter::convert_volume_to_mesh() {
    iso_surface_ = std::shared_ptr<toxi::volume::HashIsoSurface>(
        new toxi::volume::HashIsoSurface(*(volume_.get()), iso_density_));

    volume_mesh_ = std::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    iso_surface_->computeSurfaceMesh(volume_mesh_, iso_compute_);
}

void VolumePainter::update() {
    // nothing here
}

void VolumePainter::draw() {
    ci::gl::draw(*(volume_mesh_.get()));
}

}