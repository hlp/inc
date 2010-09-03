
#pragma once

#include <deque>
#include <vector>

#include <inc/Module.h>

namespace inc {
    class Solid;
    class GraphicItem;

    typedef std::vector< std::tr1::shared_ptr<Module> > ModuleList;
    typedef std::deque< std::tr1::shared_ptr<Solid> > SolidList;
    typedef std::deque< std::tr1::shared_ptr<GraphicItem> > GraphicItemList;

    class Manager : public Module {
    public:
        Manager();

        static Manager& instance();

        void setup_modules();
        void update_modules();
        void draw_modules();
        void add_module(std::tr1::shared_ptr<Module>);
        
        void setup();
        void update();
        void draw();

        ci::params::InterfaceGl& interface();

        SolidList& solids();
        GraphicItemList& graphic_items();
        ModuleList& modules();

    private:
        SolidList solids_;
        GraphicItemList graphic_items_;
        ModuleList modules_;
        ModuleList::const_iterator modules_it_;

        ci::params::InterfaceGl interface_;

        static Manager* instance_;
    };
}