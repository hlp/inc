
#include <string>

#include <cinder/gl/gl.h>
#include <cinder/app/App.h>
#include <cinder/ObjLoader.h>
#include <cinder/Color.h>

#include <inc/inc_MeshVideo.h>

namespace inc {

MeshVideo::MeshVideo() {
}

void MeshVideo::setup() {
    video_width_ = surf_width_ = 320;
    video_height_ = surf_height_ = 240;

    bound_min_ = ci::Vec3f::one() * -10.0f;
    bound_max_ = ci::Vec3f::one() * 10.0f;

    first_frame_ = false;

    // set up video stuff

    try {
        capture_ = ci::Capture(video_width_, video_height_);
        capture_.start();
    } catch (...) {
        // this should to something better than just return
        ci::app::console() << "Unable to open webcam." << std::endl;
        return;
    }

    // load mesh from file

    std::string model_name = "data/tripod-3.obj";
    ci::ObjLoader loader(ci::loadFileStream(model_name));

    mesh_ = std::shared_ptr<ci::TriMesh>(new ci::TriMesh());

    loader.load(mesh_.get(), true);

    set_bounds(*(mesh_.get()));
    build_normals();
}

void MeshVideo::update() {
    if (!capture_ || !capture_.checkNewFrame()) 
        return;

    surface_ = capture_.getSurface();

    first_frame_ = true;
}

void MeshVideo::draw() {
    if (!first_frame_)
        return;

    ci::Vec2i surf_pnt;

    glBegin(GL_TRIANGLES);

    for (int i = 0; i < mesh_->getNumTriangles(); ++i) {
        mesh_->getTriangleVertices(i, &a_, &b_, &c_);

        glColor4f(surface_.getPixel(model_to_video(a_)));
        glVertex3f(a_);

        glColor4f(surface_.getPixel(model_to_video(b_)));
        glVertex3f(b_);

        glColor4f(surface_.getPixel(model_to_video(c_)));
        glVertex3f(c_);
    }

    glEnd();
}

ci::Vec2i MeshVideo::model_to_video(ci::Vec3f model_point) {
    int y = ci::lmap<float>(model_point.y, bound_min_.y, bound_max_.y,
        0, surf_height_);

    int x = ci::lmap<float>(model_point.x, bound_min_.x, bound_max_.x,
        0, surf_width_);

    return ci::Vec2i(x, y);
}

void MeshVideo::set_bounds(const ci::TriMesh& mesh) {
    if (mesh.getNumVertices() == 0)
        return;

    ci::Vec3f min, max;

    const std::vector<ci::Vec3f>& verts = mesh.getVertices();

    min = max = verts[0];

    std::for_each (verts.begin(), verts.end(), [&] (const ci::Vec3f& v) {
        if (v.x < min.x)
            min.x = v.x;
        if (v.y < min.y)
            min.y = v.y;
        if (v.z < min.z)
            min.z = v.z;

        if (v.x > max.x)
            max.x = v.x;
        if (v.y > max.y)
            max.y = v.y;
        if (v.z > max.z)
            max.z = v.z;
    } );

    bound_min_ = min;
    bound_max_ = max;
}

void MeshVideo::build_normals() {
    ci::Vec3f d1, d2, center;

    normals_.clear();

    for (int i = 0; i < mesh_->getNumTriangles(); ++i) {
        mesh_->getTriangleVertices(i, &a_, &b_, &c_);
        
        d1 = a_ - b_;
        d2 = a_ - c_;

        center = (a_ + b_ + c_) / 3.0f;

        normals_.push_back(d1.cross(d2).normalized() + center);
    }
}

}