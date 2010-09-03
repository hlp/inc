
#pragma once

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include <cinder/app/MouseEvent.h>

#include <inc/GraphicItem.h>
#include <inc/Module.h>

namespace cinder {
    class TriMesh;
}

namespace inc {
    class Solid {
    public:
        Solid(GraphicItem*, btCollisionObject*, btDynamicsWorld*);
        virtual ~Solid();

        virtual void draw();
        virtual void set_gravity(float) = 0;
        virtual btCollisionObject& collision_object();
        virtual bool detect_selection(ci::app::MouseEvent, float* dist);

    protected:
        GraphicItem* graphic_item_;
        btCollisionObject* body_;
        btDynamicsWorld* world_;

    };

    class RigidSolid : public Solid {
    public:
        RigidSolid(GraphicItem*, btRigidBody*, btDynamicsWorld*);
        virtual ~RigidSolid();

        virtual void draw();
        virtual void set_gravity(float);
        virtual btRigidBody& rigid_body();
        virtual btRigidBody* rigid_body_ptr();
    };

    class SoftSolid : public Solid {
    public:
        SoftSolid(GraphicItem*, btSoftBody*, btDynamicsWorld*);
        virtual ~SoftSolid();

        virtual void draw();
        virtual void set_gravity(float);
        virtual btSoftBody& soft_body();
        virtual btSoftBody* soft_body_ptr();
    };

    class DebugDraw;

    typedef std::tr1::shared_ptr<Solid> SolidPtr;

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

        static SolidPtr create_solid_box(ci::Vec3f dimensions, ci::Vec3f position);
        static SolidPtr create_rigid_mesh(ci::TriMesh&, ci::Vec3f position, 
            ci::Vec3f scale, float mass);
        static SolidPtr create_soft_body(ci::TriMesh&, ci::Vec3f position,
            ci::Vec3f scale);
        static SolidPtr create_plane(ci::Vec3f dimensions, ci::Vec3f position);
        static SolidPtr create_static_solid_box(ci::Vec3f dimensions, 
            ci::Vec3f position);

        static SolidFactory& instance();

        btSoftBodyWorldInfo& soft_body_world_info();

    private:
        void init_physics();
        void update_object_gravity();

        //btDynamicsWorld* dynamics_world_;
        btSoftRigidDynamicsWorld* dynamics_world_;
        btSoftBodyWorldInfo soft_body_world_info_;
        btDefaultCollisionConstructionInfo collision_info_;

        btDefaultCollisionConfiguration* collision_configuration_;
        btCollisionDispatcher* dispatcher_;
        btBroadphaseInterface* broadphase_;
        btSequentialImpulseConstraintSolver* solver_;
        DebugDraw* debug_draw_;

        ci::params::InterfaceGl interface_;

        double time_step_;
        double last_time_;

        float gravity_;
        float last_gravity_;

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