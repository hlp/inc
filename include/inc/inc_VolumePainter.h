
#pragma once

#include <cinder/TriMesh.h>

#include <toxi/volume/toxi_volume_VolumetricRenderer.h>

#include <inc/inc_Module.h>

namespace toxi {
namespace volume {
class VolumetricSpaceVector;
class RoundBrush;
class HashIsoSurface;
}
}

namespace inc {

class VolumePainter : public Module {
public:
    VolumePainter();

    void setup();
    void update();
    void draw();

    void load_points_from_file(const std::string& file_name);
    void load_points_from_file(const std::string& file_name, float weigth);

    float brush_radius_;
    float iso_density_;
    float iso_compute_;
    float default_weight_;

private:
    void draw_volume();
    void convert_volume_to_mesh();

    std::shared_ptr<toxi::volume::VolumetricSpaceVector> volume_;
    std::shared_ptr<toxi::volume::RoundBrush> brush_;
    std::shared_ptr<toxi::volume::HashIsoSurface> iso_surface_;
    std::shared_ptr<ci::TriMesh> volume_mesh_;

    toxi::volume::VolumetricRenderer renderer_;
};

}