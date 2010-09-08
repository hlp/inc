
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

#include <cinder/params/Params.h>
#include <cinder/MayaCamUI.h>
#include <cinder/app/App.h>

#include <inc/Module.h>

namespace inc {

    class Camera : public Module, public ci::app::App::Listener {
    public:
        Camera();
        virtual ~Camera();
        
        void setup();
        void update();
        void draw();

        void set_matrices();

        // If a method returns true, event propegation stops. If it returns false,
        // the message propegates to other Listeners.
        virtual bool resize(int, int);
        virtual bool mouseDown(ci::app::MouseEvent);
        virtual bool mouseDrag(ci::app::MouseEvent);
        virtual bool mouseWheel(ci::app::MouseEvent);
        /*
        virtual bool keyDown(ci::app::KeyEvent);
        virtual bool keyUp(ci::app::KeyEvent);
        */

        static Camera& instance();

        bool draw_interface();

    private:
        void create_camera();

        ci::MayaCamUI* cam_;
        
        float zoom_speed_;
        float far_clip_;

        static Camera* instance_;
    };
}