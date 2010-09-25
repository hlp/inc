
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

#include <cinder/app/KeyEvent.h>
#include <cinder/app/App.h>

#include <inc/Module.h>
#include <inc/Widget.h>

namespace inc {

class Menu : public Module {
public:
    Menu();
    virtual ~Menu();

    virtual void setup();
    virtual void update();
    virtual void draw();

    virtual ci::params::InterfaceGl& interface();

    void add_widget(WidgetPtr);

protected:
    ci::params::InterfaceGl interface_;
    std::deque<WidgetPtr> widgets_;
};


// main menu for everything else
class MainMenu: public Menu {
public:
    MainMenu();
    virtual ~MainMenu();

    virtual void setup();

    bool save_dxf(bool); // called by a button so its passed a bool

    static MainMenu& instance();

private:
    static MainMenu* instance_;
};


// used for creating the container meshes
class MeshMenu : public Menu {
public:
    MeshMenu();
    virtual ~MeshMenu();

    virtual void setup();

    bool create_bag(bool);

    static MeshMenu& instance();

private:
    static MeshMenu* instance_;
};


// used for creating solids (rigid and soft bodies)
class SolidMenu : public Menu {
public:
    SolidMenu();
    virtual ~SolidMenu();

    virtual void setup();

    bool set_gravity(float);

    static SolidMenu& instance();

    bool create_rigid_sphere(bool);
    bool create_soft_sphere(bool);
    bool create_linked_spheres(bool);
    bool create_soft_sphere_matrix(bool);
    bool create_rigid_sphere_matrix(bool);
    bool create_rigid_sphere_spring_matrix(bool);

private:
    static SolidMenu* instance_;

    int matrix_w_;
    int matrix_h_;
    int matrix_d_;
};

class Solid;

class ForceMenu : public Menu {
public:
    static void add_menu(Solid&);
    static void remove_menu();

    virtual ~ForceMenu();

    virtual void setup();

    bool force_changed(ci::Vec3f);

    static ForceMenu& instance();

private:
    ForceMenu(Solid&);
    static ForceMenu* instance_;
    Solid& target_solid_;
};

}