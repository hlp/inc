
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
#include <string>

#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/app/KeyEvent.h>
#include <cinder/app/App.h>

#include <inc/inc_Module.h>
#include <inc/inc_Widget.h>

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

    // NOTE: this name CANNOT HAVE SPACES!! (can I enforce this somehow?)
    virtual std::string name() = 0;

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

    bool reset(bool);

    static MainMenu& instance();

    // Override
    std::string name() { return "MAIN"; }

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

    // Override
    std::string name() { return "MESH"; }

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
    bool create_soft_cylinder(bool);

    // Override
    std::string name() { return "SOLIDS"; }

    float cylinder_height_;
    float cylinder_radius_;

private:
    static SolidMenu* instance_;

    int matrix_w_;
    int matrix_h_;
    int matrix_d_;

    float sphere_radius_;
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

    // Override
    std::string name() { return "FORCES"; }

private:
    ForceMenu(Solid&);
    static ForceMenu* instance_;
    Solid& target_solid_;
};


class TripodMeshMenu : public Menu {
public:
    virtual void setup();

    bool create_mesh(bool);

    std::string name() { return "TRIPOD"; }
};


class AnemoneMeshMenu : public Menu {
public:
    virtual void setup();

    bool create_mesh(bool);

    std::string name() { return "ANEMONE"; }

};


class DrawMeshMenu : public Menu {
public:
    virtual void setup();

    std::string name() { return "DRAW"; }
};

class DisplayMenu : public Menu {
public:
    DisplayMenu();
    virtual void setup();

    std::string name() { return "DISPLAY"; }
};

class MeshNetworkMenu : public Menu {
public:
    virtual void setup();

    bool create_network(bool);

    std::string name() { return "NETWORK"; }
};

class LinkNetworkMenu : public Menu {
public:
    LinkNetworkMenu();
    virtual void setup();

    bool create_socket_matrix(bool);
    bool create_hinge_matrix(bool);
    bool create_link_mesh(bool);

    std::string name() { return "LINK"; }

private:
    ci::Vec3f hinge_axis_;
    int matrix_x_;
    int matrix_y_;
    int joint_type_;
    float joint_spacing_;
};

class FileMenu : public Menu {
public:
    FileMenu();

    virtual void setup();

    bool save_image(bool);
    bool save_high_res(bool);
    bool save_dxf(bool); // called by a button so its passed a bool

    std::string name() { return "FILE"; }

private:
    std::string get_uuid();
    std::string get_file_name();

    int image_counter_;
    int high_res_image_width_;
    bool save_uuid_; // use a uuid instead of a number

    std::string file_name_;
};


// draws the top menu tabs
class MenuManager : public Module {
public:
    MenuManager();
    virtual ~MenuManager();

    virtual void setup();
    virtual void update();
    virtual void draw();

    void delayed_draw();

    virtual bool resize(ci::app::ResizeEvent);
    virtual bool mouse_moved(ci::app::MouseEvent);

    static MenuManager& instance();

    // NOTE: (1) FORCE must be last
    // (2) this order must reflect the oder in menus_
    enum SelectedMenu {
        MAIN = 0,
        MESH = 1,
        DRAW_MESH = 2,
        TRIPOD_MESH = 3,
        ANEMONE_MESH = 4,
        MESH_NETWORK = 5,
        SOLID = 6,
        DISPLAY = 7,
        FILE = 8,
        FORCE = 9
    };

private:
    void create_menus();
    void setup_menus();
    void create_menu_texture();
    void draw_menu_texture();
    void hide_all_menus();
    void show_menu(SelectedMenu);
    bool is_inside_menu(ci::Vec2i pos);
    SelectedMenu get_hover_menu(ci::Vec2i pos);
    void position_menu();

    int x_origin_;
    int y_origin_;
    int x_spacing_;
    int y_spacing_;
    ci::Vec2f menu_pos_;

    std::vector<std::tr1::shared_ptr<Menu> > menus_;

    bool hovering_over_menu_;

    std::tr1::shared_ptr<Menu> force_menu_;

    ci::Rectf menu_rect_;
    std::vector<ci::Rectf> submenu_rects_;

    ci::gl::Texture tabs_texture_;
    std::vector<ci::gl::Texture> rollover_textures_;

    static MenuManager* instance_;

    SelectedMenu selected_menu_;

    ci::CallbackId mouse_moved_cb_id_;
    ci::CallbackId resize_cb_id_;
};

}