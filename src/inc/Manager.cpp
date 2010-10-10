
/*  Copyright (c) 2010, Patrick Tierney
 *
 *  This file is part of INC (INC's Not CAD).
 *
 *  INC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  INC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with INC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cinder/app/App.h>

#include <inc/inc_Manager.h>
#include <inc/inc_Solid.h>

namespace inc {
    Manager::Manager() {
        instance_ = this;
    }
    
    Manager::~Manager() {
#ifdef TRACE_DTORS
        ci::app::console() << "Deleting Manager" << std::endl;
#endif
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
        for (SolidList::iterator it = solids_.begin(); it != solids_.end(); ++it) {
            (*it)->update();
        }
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

    void Manager::remove_module(std::tr1::shared_ptr<Module> module) {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); ++it) {
            if (module == *it) {
                modules_.erase(it, it + 1);
                break; // erase invalidates the loop
            }
        }
    }

    void Manager::remove_solid(SolidPtr ptr) {
        for (SolidList::iterator it = solids_.begin(); it != solids_.end(); ++it) {
            if (ptr == *it) {
                solids_.erase(it, it + 1);
                break; // erase invalidates the loop
            }
        }
    }

    void Manager::add_solid(SolidPtr ptr) {
        solids_.push_back(ptr);
    }

    void Manager::clear_module_list() {
        modules_.clear();
    }

    void Manager::clear_solid_list() {
        selectable_.clear();
        solids_.clear();
    }

    void Manager::clear_graphic_item_list() {
        graphic_items_.clear();
    }

    void Manager::register_for_selection(SolidPtr solid) {
        selectable_.push_back(solid);
    }

    SolidList& Manager::get_selectable() {
        return selectable_;
    }

    void Manager::deselect_other_solids(std::tr1::shared_ptr<Solid> target) {
        SolidList& others = get_selectable();

        for (SolidList::iterator it = others.begin(); it != others.end(); ++it) {
            if (*it == target)
                continue;
            
            if ((*it)->selected())
                (*it)->select();
        }
    }
}