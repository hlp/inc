
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

#include <cinder/app/AppBasic.h>

namespace inc {
    class SolidFactory;
    class Renderer;
    class Manager;
    class Camera;
    class MenuManager;
    class ForceMenu;
    class Origin;
    class SolidCreator;
    class MeshCreator;
    class CurveSketcher;
    class VolumePainter;
    class VoronoiSurface;
}

class IncApp : public ci::app::AppBasic {
    public:
        IncApp();

        void prepareSettings(Settings*);
        void setup();
        void update();
        void draw();
        void shutdown();

        static IncApp& instance();

        // cleaned up (reset) in the shutdown() method
        std::tr1::shared_ptr<inc::ForceMenu> force_menu_;

        void set_draw_interface(bool);

    private:
        // Access these with T::instance()
        std::shared_ptr<inc::SolidFactory> solid_factory_;
        std::shared_ptr<inc::Renderer> renderer_;
        std::shared_ptr<inc::Manager> manager_;
        std::shared_ptr<inc::Camera> camera_;
        std::shared_ptr<inc::MenuManager> menu_manager_;
        std::shared_ptr<inc::Origin> origin_;
        std::shared_ptr<inc::SolidCreator> solid_creator_;
        std::shared_ptr<inc::MeshCreator> mesh_creator_;
        std::shared_ptr<inc::CurveSketcher> curve_sketcher_;
        std::shared_ptr<inc::VoronoiSurface> voronoi_surface_;

        bool draw_interface_;

        static IncApp* instance_;
};