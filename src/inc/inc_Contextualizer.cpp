
#include <cinder/gl/gl.h>
#include <cinder/ObjLoader.h>

#include <inc/inc_Contextualizer.h>
#include <inc/inc_Manager.h>
#include <inc/inc_GraphicItem.h>

namespace inc {

Contextualizer::Contextualizer() {
}

Contextualizer::~Contextualizer() {
    std::for_each(shaded_meshes_.begin(), shaded_meshes_.end(),
        [] (std::shared_ptr<ShadedMesh> ptr) {
            Manager::instance().remove_graphic_item(ptr);
    } );
}

void Contextualizer::load_model(std::string model_name) {
    ci::ObjLoader loader(ci::loadFileStream(model_name));

    std::shared_ptr<ci::TriMesh> load_mesh(new ci::TriMesh());

    loader.load(load_mesh.get(), true);

    meshes_.push_back(load_mesh);

    std::shared_ptr<ShadedMesh> shaded = std::shared_ptr<ShadedMesh>(
        new ShadedMesh(load_mesh));

    shaded_meshes_.push_back(shaded);
    
    Manager::instance().add_graphic_item(shaded);
}

void Contextualizer::load_model(const ci::TriMesh& mesh) {
    std::shared_ptr<ci::TriMesh> load_mesh(new ci::TriMesh(mesh));

    meshes_.push_back(load_mesh);

    std::shared_ptr<ShadedMesh> shaded = std::shared_ptr<ShadedMesh>(
        new ShadedMesh(load_mesh));

    shaded_meshes_.push_back(shaded);

    Manager::instance().add_graphic_item(shaded);
}


void Contextualizer::setup() {
    position_ = ci::Vec3f::zero();
    scale_ = ci::Vec3f::one();
    rotation_ = ci::Vec3f::zero();

    load_model("/data/context3.obj");
}

void Contextualizer::update() {
    // nothing here
}

void Contextualizer::draw() {
    // nothing here        
}




}