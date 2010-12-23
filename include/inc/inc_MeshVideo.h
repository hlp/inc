
#pragma once

#include <cinder/Vector.h>
#include <cinder/Capture.h>
#include <cinder/Surface.h>

#include <inc/inc_Module.h>

namespace inc {

class MeshVideo : public Module {
public:
    MeshVideo();

    void setup();
    void update();
    void draw();

private:
    // Takes a point in model space (eg 3D coords), and maps it to
    // a surface point. 
    // Ignores z coordinate at the moment
    ci::Vec2i model_to_video(ci::Vec3f model_point);
    void set_bounds(const ci::TriMesh& mesh);
    void build_normals();


    ci::Capture capture_;
    ci::Surface surface_;
    std::shared_ptr<ci::TriMesh> mesh_;

    // the area to 
    ci::Vec3f bound_min_;
    ci::Vec3f bound_max_;

    int video_width_;
    int video_height_;
    
    int surf_width_;
    int surf_height_;

    bool first_frame_;

    std::vector<ci::Vec3f> normals_;

    // for draw()
    ci::Vec3f a_, b_, c_;
};

}