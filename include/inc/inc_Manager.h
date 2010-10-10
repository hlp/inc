
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

#pragma once

#include <deque>
#include <vector>

#include <inc/inc_Module.h>

namespace inc {
class Solid;
class GraphicItem;

typedef std::deque< std::tr1::shared_ptr<Module> > ModuleList;
typedef std::deque< std::tr1::shared_ptr<Solid> > SolidList;
typedef std::deque< std::tr1::shared_ptr<GraphicItem> > GraphicItemList;

class Manager : public Module {
public:
    Manager();
    virtual ~Manager();

    static Manager& instance();

    void setup_modules();
    void update_modules();
    void draw_modules();
    void add_module(std::tr1::shared_ptr<Module>);
    void remove_module(std::tr1::shared_ptr<Module>);
    void add_solid(std::tr1::shared_ptr<Solid>);
    void remove_solid(std::tr1::shared_ptr<Solid>);
        
    void setup();
    void update();
    void draw();

    void clear_module_list();
    void clear_solid_list();
    void clear_graphic_item_list();

    void register_for_selection(std::tr1::shared_ptr<Solid>);

    SolidList& get_selectable(); // returns the items the camera should check
    void deselect_other_solids(std::tr1::shared_ptr<Solid>);

    //ci::params::InterfaceGl& interface();

    SolidList& solids();
    GraphicItemList& graphic_items();
    ModuleList& modules();

private:
    SolidList solids_;
    SolidList selectable_;
    GraphicItemList graphic_items_;
    ModuleList modules_;
    ModuleList::const_iterator modules_it_;

    //ci::params::InterfaceGl interface_;

    static Manager* instance_;
};

}