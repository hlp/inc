
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <cinder/gl/gl.h>
#include <cinder/app/App.h>

#include <toxi/volume/toxi_volume_VolumetricSpaceVector.h>
#include <toxi/volume/toxi_volume_RoundBrush.h>
#include <toxi/volume/toxi_volume_HashIsoSurface.h>

#include <inc/inc_VolumePainter.h>
#include <inc/inc_GraphicItem.h>
#include <inc/inc_Manager.h>

namespace inc {

VolumePainter::VolumePainter() {
    brush_radius_ = 20.0f;
    iso_density_ = 1.0f;
    iso_compute_ = 3.0f * 0.001f;
    default_weight_ = 3.0f;
}

void VolumePainter::setup() {
    // 1st param is the bounding box that the VolumetricSpace covers
    // it is mirrored into the negative axis
    // 2nd, 3rd, 4th params are the resolution along each axis
    volume_ = std::shared_ptr<toxi::volume::VolumetricSpaceVector>(new 
        toxi::volume::VolumetricSpaceVector(ci::Vec3f::one() * 1000.0f,
        500.f, 500.0f, 500.0f));

    brush_ = std::shared_ptr<toxi::volume::RoundBrush>(new
        toxi::volume::RoundBrush(*(volume_.get()), brush_radius_));

    load_points_from_file("/data/points5.txt");

    //brush_->drawAtAbsolutePos(ci::Vec3f::one() * -20.0f, default_weight_);

    convert_volume_to_mesh();

    if (volume_mesh_.get() == NULL)
        return;

    // cinder was throwing an error when drawing an empty mesh
    if (volume_mesh_->getNumTriangles() == 0)
        return;

    shaded_mesh_ = std::shared_ptr<ShadedMesh>(new ShadedMesh(volume_mesh_));
    shaded_mesh_->set_shade(false);
    shaded_mesh_->set_draw_wireframe(true);
    shaded_mesh_->set_color(ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f));

    Manager::instance().add_graphic_item(shaded_mesh_);
}

VolumePainter::~VolumePainter() {
    Manager::instance().remove_graphic_item(shaded_mesh_);
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
    // nothing here
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

}