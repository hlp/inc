
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

#include <cinder/app/KeyEvent.h>
#include <cinder/app/App.h>

#include <inc/Module.h>

namespace inc {

class Menu : public Module, public ci::app::App::Listener {
public:
    Menu();
    virtual ~Menu();

    void setup();
    void update();
    void draw();

    virtual bool keyDown(ci::app::KeyEvent);
    virtual bool keyUp(ci::app::KeyEvent);

    Menu& instance();

private:
    static Menu* instance_;
};

}