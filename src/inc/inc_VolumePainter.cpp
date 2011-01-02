
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <cinder/gl/gl.h>
#include <cinder/app/App.h>

#include <toxi/volume/toxi_volume_VolumetricSpaceVector.h>
#include <toxi/volume/toxi_volume_RoundBrush.h>
#include <toxi/volume/toxi_volume_HashIsoSurface.h>
#include <toxi/volume/toxi_volume_MeshVoxelizer.h>

#include <inc/inc_VolumePainter.h>
#include <inc/inc_GraphicItem.h>
#include <inc/inc_Manager.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Camera.h>
#include <incApp.h>

namespace inc {

VolumePainter::VolumePainter() {
    brush_radius_ = 8.0f;
    iso_density_ = 1.0f;
    iso_compute_ = 3.0f * 0.001f;
    default_weight_ = 5.0f;

    dragged_ = false;
}

VolumePainter::~VolumePainter() {
#ifdef TRACE_DTORS
        ci::app::console() << "Deleting VolumePainter" << std::endl;
#endif

    IncApp::instance().unregisterMouseDrag(mouse_drag_cb_id_);
    IncApp::instance().unregisterMouseUp(mouse_up_cb_id_);

    Manager::instance().remove_graphic_item(shaded_mesh_);
    Manager::instance().remove_solid(soft_solid_);
}

void VolumePainter::setup() {
    mouse_drag_cb_id_ = IncApp::instance().registerMouseDrag(this, 
        &VolumePainter::mouse_drag);
    mouse_up_cb_id_ = IncApp::instance().registerMouseUp(this, 
        &VolumePainter::mouse_up);

    volumetric_renderer_ = std::shared_ptr<toxi::volume::VolumetricRenderer>(
        new toxi::volume::VolumetricRenderer());

    // 1st param is the bounding box that the VolumetricSpace covers
    // it is mirrored into the negative axis
    // 2nd, 3rd, 4th params are the resolution along each axis
    volume_ = std::shared_ptr<toxi::volume::VolumetricSpaceVector>(new 
        toxi::volume::VolumetricSpaceVector(ci::Vec3f::one() * 100.0f,
        40, 40, 40));

    brush_ = std::shared_ptr<toxi::volume::RoundBrush>(new
        toxi::volume::RoundBrush(*(volume_.get()), brush_radius_));

    mesh_voxelizer_ = std::shared_ptr<toxi::volume::MeshVoxelizer>(
        new toxi::volume::MeshVoxelizer(40));

    load_points_from_file("/data/points6.txt");

    //brush_->drawAtAbsolutePos(ci::Vec3f::one() * -20.0f, default_weight_);

    volume_mesh_ = std::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    convert_volume_to_mesh(volume_, volume_mesh_);

    if (volume_mesh_.get() == NULL)
        return;

    // cinder was throwing an error when drawing an empty mesh
    if (volume_mesh_->getNumTriangles() == 0)
        return;

    // create new bullet object from trimesh

    soft_solid_ = SolidFactory::create_soft_mesh(
        volume_mesh_);

    Manager::instance().add_solid(soft_solid_);

    shaded_mesh_ = std::shared_ptr<ShadedMesh>(new ShadedMesh(soft_solid_->get_mesh()));
    shaded_mesh_->set_shade(false);
    shaded_mesh_->set_draw_wireframe(true);
    shaded_mesh_->set_color(ci::ColorA(0.6f, 0.6f, 0.6f, 1.0f));

    Manager::instance().add_graphic_item(shaded_mesh_);
    
}

void VolumePainter::convert_volume_to_mesh(std::shared_ptr<toxi::volume::VolumetricSpace>
    volume, std::shared_ptr<ci::TriMesh> mesh) {
    iso_surface_ = std::shared_ptr<toxi::volume::HashIsoSurface>(
        new toxi::volume::HashIsoSurface(*(volume.get())));

    iso_surface_->computeSurfaceMesh(volume_mesh_, iso_compute_);
}

void VolumePainter::update() {
    // nothing here
}

void VolumePainter::draw() {
    if (volume_.get() == NULL)
        return;

    volumetric_renderer_->drawVolume(*(volume_.get()));
}


// NOTE: assumed points are comma comma separated, one point per line
void VolumePainter::load_points_from_file(const std::string& file_name) {
    load_points_from_file(file_name, default_weight_);
}

void VolumePainter::load_points_from_file(const std::string& file_name, float w) {
    // load in the file
    ci::IStreamFileRef r = ci::loadFileStream(file_name);
    
    std::vector<std::string> lines;

    // split into lines
    while (!r->isEof()) {
        lines.push_back(r->readLine());
    }

    std::vector<std::string> string_points;
    std::vector<ci::Vec3f> points;

    // split at a comma
    for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end();
        ++it) {
        string_points.clear();
        boost::split(string_points, *it, boost::is_any_of(","));
        
        // now try to convert these to points
        try {
            points.push_back(ci::Vec3f(
                boost::lexical_cast<float>(string_points[0]),
                boost::lexical_cast<float>(string_points[2]),
                -boost::lexical_cast<float>(string_points[1])));
        } catch (...) {
            ci::app::console() << "Invalid point input" << std::endl;
        }
    }

    for (std::vector<ci::Vec3f>::const_iterator it = points.begin(); it != points.end();
        ++it) {
        brush_->drawAtAbsolutePos(*it, w);
    }
}

bool VolumePainter::mouse_drag(ci::app::MouseEvent evt) {
    dragged_ = true;

    return false;
}

////////////////////////////
// NOTE: I think the problem is with toxi_geom_AABB, in that it is returning
// false positives in voxel.intersectsTriangle(), in MeshVoxelizer
////////////////////////////

bool VolumePainter::mouse_up(ci::app::MouseEvent evt) {
    if (dragged_) {
        dragged_ = false;
        return false;
    }

    ci::app::console() << "Click" << std::endl;

    ci::Ray click_ray = Camera::instance().get_ray_from_screen_pos(evt.getPos());

    std::shared_ptr<ci::TriMesh> mesh = soft_solid_->get_mesh();

    // clear all existing voxels
    mesh_voxelizer_->clear();

    // build voxel shape from mesh
    mesh_voxelizer_->voxelizeMesh(*(mesh.get()));

    // test intersections with squares
    // paint voxels

    // mesh voxels
    std::shared_ptr<toxi::volume::VolumetricSpace> vol = mesh_voxelizer_->getVolume();

    volume_ = std::dynamic_pointer_cast<toxi::volume::VolumetricSpaceVector>(vol);

    std::shared_ptr<ci::TriMesh> new_mesh(new ci::TriMesh());

    convert_volume_to_mesh(vol, new_mesh);

    ci::app::console() << new_mesh->getNumTriangles() << std::endl;

    if (new_mesh->getNumTriangles() == 0) {
        ci::app::console() << "Meshing failed" << std::endl;
        return false;
    }

    Manager::instance().remove_solid(soft_solid_);

    soft_solid_.reset();

    soft_solid_ = SolidFactory::create_soft_mesh(
        new_mesh);

    Manager::instance().add_solid(soft_solid_);

    dragged_ = false;
    return true;
}

}