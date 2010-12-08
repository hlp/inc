

#pragma once

#include <string>

#include <cinder/TriMesh.h>
#include <cinder/Vector.h>

#include <inc/inc_Module.h>

// handles placing a context around your model (ie the surrounding 
// buildings, the topo

namespace inc {

class ShadedMesh;

class Contextualizer : public Module {
public:
    Contextualizer();
    virtual ~Contextualizer();

    // TODO: give each load event its own position, rotation, scale
    void load_model(std::string model_name);
    void load_model(const ci::TriMesh& mesh);

    void setup();
    void update();
    void draw();

    // TODO: make these private after each model is scaled individually
    ci::Vec3f position_;
    ci::Vec3f scale_;
    ci::Vec3f rotation_;

private:
    std::vector<std::shared_ptr<ci::TriMesh>> meshes_;
    std::vector<std::shared_ptr<ShadedMesh>> shaded_meshes_;
};

}