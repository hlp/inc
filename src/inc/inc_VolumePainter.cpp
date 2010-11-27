
#include <cinder/gl/gl.h>
#include <cinder/app/App.h>

#include <toxi/volume/toxi_volume_VolumetricSpaceVector.h>
#include <toxi/volume/toxi_volume_RoundBrush.h>
#include <toxi/volume/toxi_volume_HashIsoSurface.h>

#include <inc/inc_VolumePainter.h>

namespace inc {

VolumePainter::VolumePainter() {
    brush_radius_ = 1.0f;
    iso_density_ = 1.0f;
    iso_compute_ = 3.0f * 0.001f;
}

void VolumePainter::setup() {
    // 1st param is the bounding box that the VolumetricSpace covers
    // it is mirrored into the negative axis
    // 2nd, 3rd, 4th params are the resolution along each axis
    volume_ = std::shared_ptr<toxi::volume::VolumetricSpaceVector>(new 
        toxi::volume::VolumetricSpaceVector(ci::Vec3f::one() * 50.0f,
        45.f, 45.0f, 45.0f));

    brush_ = std::shared_ptr<toxi::volume::RoundBrush>(new
        toxi::volume::RoundBrush(*(volume_.get()), brush_radius_));

    float d = 3.0f;

    /*
    for (float f = -20.0f; f < 30.0f; f += 30.0f) {
        brush_->drawAtAbsolutePos(ci::Vec3f::one() * f, d);
    }
    */

    brush_->drawAtAbsolutePos(ci::Vec3f::one() * -20.0f, d);
    brush_->drawAtAbsolutePos(ci::Vec3f::one() * 20.0f, d);
    brush_->drawAtAbsolutePos(ci::Vec3f(20, 10, 5), d);
    //brush_->drawAtAbsolutePos(ci::Vec3f(50.0f, 0.f, 0.f), d);
    //brush_->drawAtAbsolutePos(ci::Vec3f(-50.0f, 0.f, 0.f), d);

    //brush_->drawAtGridPos(90, 180, 90, d);
    //brush_->drawAtGridPos(90, 0, 90, d);

    convert_volume_to_mesh();
}

void VolumePainter::convert_volume_to_mesh() {
    iso_surface_ = std::shared_ptr<toxi::volume::HashIsoSurface>(
        new toxi::volume::HashIsoSurface(*(volume_.get())));

    volume_mesh_ = std::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    iso_surface_->computeSurfaceMesh(volume_mesh_, iso_compute_);
}

void VolumePainter::update() {
    // nothing here
}

void VolumePainter::draw() {
    if (volume_mesh_.get() == NULL)
        return;

    // cinder was throwing an error when drawing an empty mesh
    if (volume_mesh_->getNumTriangles() == 0)
        return;

    renderer_.drawVolume(*(volume_.get()));
    
    ci::gl::enableWireframe();
    ci::gl::color(ci::Color::white());
    ci::gl::draw(*(volume_mesh_.get()));
    ci::gl::disableWireframe();
}

}