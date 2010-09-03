
#include <inc/Manager.h>

namespace inc {
    Manager::Manager() {
        instance_ = this;
    }

    void Manager::setup() {
        // Nothing here
    }

    void Manager::setup_modules() {
        for (ModuleList::const_iterator it = modules_.begin(); it != modules_.end(); ++it) {
            (*it)->setup();
        }
    }

    Manager* Manager::instance_;

    Manager& Manager::instance() {
        return *instance_;
    }

    SolidList& Manager::solids() {
        return solids_;
    }

    GraphicItemList& Manager::graphic_items() {
        return graphic_items_;
    }

    void Manager::update() {
        // Nothing here
    }

    void Manager::update_modules() {
        for (ModuleList::const_iterator it = modules_.begin(); it != modules_.end(); ++it) {
            (*it)->update();
        }
    }

    void Manager::draw() {
        // Nothing here
    }

    void Manager::draw_modules() {
        for (ModuleList::const_iterator it = modules_.begin(); it != modules_.end(); ++it) {
            (*it)->draw();
        }
    }

    void Manager::add_module(std::tr1::shared_ptr<Module> module) {
        modules_.push_back(module);
    }
}