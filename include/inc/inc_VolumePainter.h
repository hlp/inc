
#pragma once

#include <cinder/TriMesh.h>
#include <cinder/app/App.h>

#include <CiIso.h>

#include <toxi/volume/toxi_volume_VolumetricRenderer.h>

#include <inc/inc_Module.h>

namespace toxi {
namespace volume {
class VolumetricSpace;
class VolumetricSpaceVector;
class RoundBrush;
class HashIsoSurface;
class MeshVoxelizer;
class VolumetricRenderer;
}
}

namespace inc {

class ShadedMesh;
class SoftSolid;

class VolumePainter : public Module {
public:
    VolumePainter();
    virtual ~VolumePainter();

    void setup();
    void update();
    void draw();

    virtual bool mouse_drag(ci::app::MouseEvent);
    virtual bool mouse_up(ci::app::MouseEvent);

    void load_points_from_file(const std::string& file_name);
    void load_points_from_file(const std::string& file_name, float weigth);

    float brush_radius_;
    float iso_density_;
    float iso_compute_;
    float default_weight_;

private:
    void draw_volume();
    void convert_volume_to_mesh(std::shared_ptr<toxi::volume::VolumetricSpace>,
        std::shared_ptr<ci::TriMesh>);

    std::shared_ptr<toxi::volume::VolumetricSpaceVector> volume_;
    std::shared_ptr<toxi::volume::RoundBrush> brush_;
    std::shared_ptr<toxi::volume::HashIsoSurface> iso_surface_;
    std::shared_ptr<toxi::volume::MeshVoxelizer> mesh_voxelizer_;
    std::shared_ptr<toxi::volume::VolumetricRenderer> volumetric_renderer_;
    std::shared_ptr<ci::TriMesh> volume_mesh_;
    std::shared_ptr<ShadedMesh> shaded_mesh_;
    std::shared_ptr<SoftSolid> soft_solid_;

    toxi::volume::VolumetricRenderer renderer_;

    CiIsoParticleSysf   particles_;
	CiIsoPolygonizerf   polygonizer_;

    // Click stuff
    bool dragged_;

    ci::CallbackId mouse_drag_cb_id_;
    ci::CallbackId mouse_up_cb_id_;

    ci::Vec2i mouse_pos_;
};

}