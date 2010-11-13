
/*  Copyright (C) 2010 Patrick Tierney
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

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include <cinder/app/MouseEvent.h>

#include <inc/inc_GraphicItem.h>
#include <inc/inc_Module.h>

namespace cinder {
class TriMesh;
}

namespace inc {
class Exporter;
    
class Solid {
public:
    Solid(SolidGraphicItem*, btCollisionObject*, btDynamicsWorld*);
    virtual ~Solid();

    static bool allow_selection_;
    static bool allow_forces_;

    virtual void update();
    virtual void draw();
    virtual void save(Exporter&) = 0;
    virtual void set_gravity(float) = 0;
    virtual void set_force(ci::Vec3f) = 0; // a constant force
    virtual void remove_force();
    virtual ci::Vec3f* force_ptr(); // the force menu hooks into this
    virtual btCollisionObject& collision_object();

    virtual bool detect_selection(ci::Ray); 
    virtual void select();   

    bool selected();
    void set_selected(bool);
    bool visible();
    void set_visible(bool);

protected:
    SolidGraphicItem* graphic_item_;
    btCollisionObject* body_;
    btDynamicsWorld* world_;
    bool selected_;
    bool has_force_;
    bool visible_;
    ci::Vec3f force_;

};

class RigidSolid : public Solid {
public:
    RigidSolid(SolidGraphicItem*, btRigidBody*, btDynamicsWorld*);
    virtual ~RigidSolid();

    virtual ci::Vec3f position();

    virtual void draw();
    virtual void save(Exporter&);
    virtual void set_gravity(float);
    virtual void set_force(ci::Vec3f);
    virtual btRigidBody& rigid_body();
    virtual btRigidBody* rigid_body_ptr();
};

class SoftSolid : public Solid {
public:
    SoftSolid(SolidGraphicItem*, btSoftBody*, btDynamicsWorld*);
    virtual ~SoftSolid();

    // Override
    virtual void update();
    virtual void draw();
    virtual void save(Exporter&);
    virtual void set_gravity(float);
    virtual void set_force(ci::Vec3f);
    virtual btSoftBody& soft_body();
    virtual btSoftBody* soft_body_ptr();

    // Override
    virtual void set_selected(bool);
    // Override
    virtual void select();

    // Override
    virtual bool detect_selection(ci::Ray);
};

class DebugDraw;

typedef std::tr1::shared_ptr<Solid> SolidPtr;
typedef std::tr1::shared_ptr<RigidSolid> RigidSolidPtr;
typedef std::tr1::shared_ptr<SoftSolid> SoftSolidPtr;

class SolidFactory : public Module {
public:
    SolidFactory();
    ~SolidFactory();

    void setup();
    void update();
    void draw();

    float time_step();
    btDynamicsWorld* dynamics_world();
    btSoftRigidDynamicsWorld* soft_dynamics_world();
    float gravity();

    // menu hooks
    bool set_gravity(float);
    float* gravity_ptr();

    void update_object_gravity(); // this applies any gravity changes to all objects
        
    static RigidSolidPtr create_rigid_sphere(ci::Vec3f position, ci::Vec3f radius);
    static SoftSolidPtr create_soft_sphere(ci::Vec3f position, ci::Vec3f radius);
    static std::tr1::shared_ptr<std::deque<SolidPtr> > create_linked_soft_spheres(
        ci::Vec3f pos, ci::Vec3f radius);
    static std::tr1::shared_ptr<std::deque<SolidPtr> > create_soft_sphere_matrix(
        ci::Vec3f pos, ci::Vec3f radius, int w, int h, int d);
    static std::tr1::shared_ptr<std::deque<SolidPtr> > create_rigid_sphere_matrix(
        ci::Vec3f pos, ci::Vec3f radius, int w, int h, int d);
    static std::tr1::shared_ptr<std::deque<SolidPtr> > create_rigid_sphere_spring_matrix(
        ci::Vec3f pos, ci::Vec3f radius, int w, int h, int d);
    static SoftSolidPtr create_soft_mesh(std::tr1::shared_ptr<ci::TriMesh>,
        ci::Vec3f scl = ci::Vec3f::one(), bool lock_base_vertices = true);

    SoftSolidPtr create_soft_container_from_convex_hull(
        std::tr1::shared_ptr<std::vector<ci::Vec3f>> points, 
        bool lock_base_vertices = true);

    // I don't use these
    static SolidPtr create_plane(ci::Vec3f dimensions, ci::Vec3f position);
    static SolidPtr create_static_solid_box(ci::Vec3f dimensions, 
        ci::Vec3f position);
    static SolidPtr create_solid_box(ci::Vec3f dimensions, 
        ci::Vec3f position);
    static SolidPtr create_rigid_mesh(ci::TriMesh&, 
        ci::Vec3f position, 
        ci::Vec3f scale, float mass);
    static btRigidBody* create_bullet_rigid_sphere(ci::Vec3f position,
        float radius);

    static SolidFactory& instance();
    static SolidFactory* instance_ptr();

    btSoftBodyWorldInfo& soft_body_world_info();

    void delete_constraints();


    // menu hooks
    bool adjust_kDF(float);
    bool adjust_kDP(float);
    bool adjust_kDG(float);
    bool adjust_kPR(float);
    bool adjust_kMT(float);

    float* kDF_ptr() { return &kDF_; }
    float* kDP_ptr() { return &kDP_; }
    float* kDG_ptr() { return &kDG_; }
    float* kPR_ptr() { return &kPR_; }
    float* kMT_ptr() { return &kMT_; }

    float* sphere_kLST_ptr() { return &sphere_kLST_; }
    float* sphere_kVST_ptr() { return &sphere_kVST_; }
    float* sphere_kDF_ptr() { return &sphere_kDF_; }
    float* sphere_kDP_ptr() { return &sphere_kDP_; }
    float* sphere_kPR_ptr() { return &sphere_kPR_; }
    float* sphere_total_mass_ptr() { return &sphere_total_mass_; }

    bool draw_bullet_debug_;

private:
    void init_physics();
    bool physics_param_changed();
        
    static btSoftBody* create_bullet_soft_sphere(ci::Vec3f position, 
        ci::Vec3f radius, float res);
    static void socket_link_soft_spheres(btSoftBody* s1, btSoftBody* s2,
        const ci::Vec3f& p1, const ci::Vec3f& p2);
    static void spring_link_rigid_spheres(btRigidBody* r1, btRigidBody* r2,
        const ci::Vec3f& p1, const ci::Vec3f& p2);


    static std::tr1::shared_ptr<ci::TriMesh> remove_mesh_duplicates(
        const ci::TriMesh& mesh);
    static std::tr1::shared_ptr<std::vector<int> > get_top_vertices(
        const ci::TriMesh& mesh);

    //btDynamicsWorld* dynamics_world_;
    btSoftRigidDynamicsWorld* dynamics_world_;
    btSoftBodyWorldInfo soft_body_world_info_;
    btDefaultCollisionConstructionInfo collision_info_;

    btDefaultCollisionConfiguration* collision_configuration_;
    btCollisionDispatcher* dispatcher_;
    btBroadphaseInterface* broadphase_;
    btSequentialImpulseConstraintSolver* solver_;
    DebugDraw* debug_draw_;

    static std::deque<btTriangleMesh*> mesh_cleanup_;

    static ci::ColorA sphere_color_;
    static ci::ColorA container_color_;

    double time_step_;
    double last_time_;

    float gravity_;
    float last_gravity_;

    static float kDF_;
    static float kDP_;
    static float kDG_;
    static float kPR_;
    static float kMT_;

    static float sphere_kLST_;
    static float sphere_kVST_;
    static float sphere_kDF_;
    static float sphere_kDP_;
    static float sphere_kPR_;
    static float sphere_total_mass_;

    static SolidFactory* instance_;

    
};

/*
enum	DebugDrawModes
{
	DBG_NoDebug=0,
	DBG_DrawWireframe = 1,
	DBG_DrawAabb=2,
	DBG_DrawFeaturesText=4,
	DBG_DrawContactPoints=8,
	DBG_NoDeactivation=16,
	DBG_NoHelpText = 32,
	DBG_DrawText=64,
	DBG_ProfileTimings = 128,
	DBG_EnableSatComparison = 256,
	DBG_DisableBulletLCP = 512,
	DBG_EnableCCD = 1024,
	DBG_DrawConstraints = (1 << 11),
	DBG_DrawConstraintLimits = (1 << 12),
	DBG_FastWireframe = (1<<13),
	DBG_MAX_DEBUG_DRAW_MODE
};
*/

class DebugDraw : public btIDebugDraw {
public:
    DebugDraw();
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, 
        btScalar distance, int lifeTime, const btVector3& color);
    void reportErrorWarning(const char* text);

    void draw3dText(const btVector3& location, const char* text);
    void setDebugMode(int mode);
    int getDebugMode() const; 

private:
    int mode_;

};

}