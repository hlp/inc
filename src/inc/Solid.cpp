
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

#include <math.h>

#include <LinearMath/btIDebugDraw.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>

// Required for CinderBullet.h
#include <cinder/app/AppBasic.h>
#include <cinder/Surface.h>
#include <cinder/gl/Material.h>
#include <cinder/gl/Light.h>
#include <cinder/Camera.h>
#include <cinder/AxisAlignedBox.h>
#include <cinder/Sphere.h>
#include <cinder/gl/GlslProg.h>
// N.B. CinderBullet.h is more like a .cpp, in can only be included once in a project
#include <blocks/bullet/CinderBullet.h>
#include <cinder/Vector.h>
#include <cinder/Quaternion.h>
#include <cinder/app/MouseEvent.h>
#include <cinder/Matrix.h>
#include <cinder/TriMesh.h>
#include <cinder/Rand.h>
#include <cinder/ObjLoader.h>

#include <inc/Solid.h>
#include <inc/GraphicItem.h>
#include <inc/Manager.h>
#include <inc/BunnyMesh.h>

namespace inc {
    Solid::Solid(GraphicItem* item, btCollisionObject* body, btDynamicsWorld* world) 
        : graphic_item_(item), body_(body), world_(world) {
        ci::app::console() << "Creating Solid" << std::endl;
    }

    Solid::~Solid() {
        delete graphic_item_;

        delete body_;
    }

    void Solid::draw() {
        return;

        graphic_item_->draw();
    }

    btCollisionObject& Solid::collision_object() {
        return *body_;
    }

    bool Solid::detect_selection(ci::app::MouseEvent event, float* distance) {
        return false;
    }


    RigidSolid::RigidSolid(GraphicItem* item, btRigidBody* body, btDynamicsWorld* world) 
        : Solid(item, body, world) {
    }

    RigidSolid::~RigidSolid() {
        if (rigid_body().getMotionState())
            delete rigid_body().getMotionState();
        world_->removeRigidBody(rigid_body_ptr());
    }

    void RigidSolid::draw() {
        ci::Matrix44f tf = ci::bullet::getWorldTransform(rigid_body_ptr());

        glPushMatrix();
            glMultMatrixf(tf.m);
            Solid::draw();
        glPopMatrix();
    }

    void RigidSolid::set_gravity(float g) {
        rigid_body().setGravity(btVector3(0.0, g, 0.0));
        body_->activate();
    }

    // TODO: fix this
    btRigidBody& RigidSolid::rigid_body() {
        return *((btRigidBody*)body_);
    }

    btRigidBody* RigidSolid::rigid_body_ptr() {
        return (btRigidBody*)body_;
    }



    SoftSolid::SoftSolid(GraphicItem* item, btSoftBody* body, btDynamicsWorld* world) 
        : Solid(item, body, world) {
    }

    SoftSolid::~SoftSolid() {
        world_->removeCollisionObject(body_);
    }

    void SoftSolid::draw() {
        Solid::draw();
    }

    // TODO: actually set gravity
    void SoftSolid::set_gravity(float g) {
        body_->activate();
    }

    // TODO: fix this
    btSoftBody& SoftSolid::soft_body() {
        return *((btSoftBody*)body_);
    }

    btSoftBody* SoftSolid::soft_body_ptr() {
        return (btSoftBody*)body_;
    }



    SolidFactory::SolidFactory() {
        instance_ = this;
        gravity_ = -9.8f;//0.0f;//-9.8f;
        last_gravity_ = gravity_;
    }

    void SolidFactory::setup() {
        init_physics();

        interface_ = ci::params::InterfaceGl("Solids_Factory", ci::Vec2i(100, 200));
        interface_.addParam("Gravity", &gravity_, 
            "min=-20.0 max=20.0 step=0.1 keyIncr=g keyDecr=G");
    }

    void SolidFactory::init_physics() {
        collision_configuration_ = new btSoftBodyRigidBodyCollisionConfiguration();
        int max_proxies = 32766;
        btVector3 world_aabb_min(-300,-300,-300);
	    btVector3 world_aabb_max(300,300,300);
        broadphase_ = new btAxisSweep3(world_aabb_min, world_aabb_max, max_proxies);
        soft_body_world_info_.m_broadphase = broadphase_;

        dispatcher_ = new btCollisionDispatcher(collision_configuration_);
        soft_body_world_info_.m_dispatcher = dispatcher_;

        solver_ = new btSequentialImpulseConstraintSolver();

        dynamics_world_ = new btSoftRigidDynamicsWorld(dispatcher_, broadphase_, 
            solver_, collision_configuration_);

        dynamics_world_->setGravity(btVector3(0, gravity_, 0));
        soft_body_world_info_.m_gravity = btVector3(0, gravity_, 0);

        soft_body_world_info_.m_sparsesdf.Initialize();

        debug_draw_ = new DebugDraw();
        debug_draw_->setDebugMode(
            btIDebugDraw::DBG_DrawWireframe);
            //btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb);
        dynamics_world_->setDebugDrawer(debug_draw_);
    }

    void SolidFactory::update() {
        if (gravity_ != last_gravity_) {
            dynamics_world_->setGravity(btVector3(0, gravity_, 0));
            soft_body_world_info_.m_gravity = btVector3(0, gravity_, 0);
            update_object_gravity();

            last_gravity_ = gravity_;
        }

        dynamics_world_->stepSimulation(1.0f, 10);
	
        double now = ci::app::getElapsedSeconds();
	    double time_step_ = now - last_time_;
	    last_time_ = now;
    }

    void SolidFactory::draw() {
        glLineWidth(0.9f);
        glBegin(GL_LINES);
        dynamics_world_->debugDrawWorld();
        glEnd();
    }

    SolidFactory::~SolidFactory() {
        delete debug_draw_;

        delete dynamics_world_;

        delete solver_;

        delete broadphase_;

        delete dispatcher_;

        delete collision_configuration_;
    }

    SolidPtr SolidFactory::create_solid_box(ci::Vec3f dimensions, 
        ci::Vec3f position) {
        btRigidBody* body = ci::bullet::createBox(SolidFactory::instance().dynamics_world(), 
            dimensions, ci::Quatf(), position);

        GraphicItem* item = new BoxGraphicItem(dimensions);

        SolidPtr solid(new RigidSolid(item, body, 
            SolidFactory::instance().dynamics_world()));

        return solid;
    }

    SolidPtr SolidFactory::create_rigid_mesh(ci::TriMesh& mesh, 
        ci::Vec3f position, ci::Vec3f scale, float mass) {
        // Create bullet object
        btConvexHullShape* shape = ci::bullet::createConvexHullShape(mesh, scale);

        btRigidBody* body = ci::bullet::createConvexHullBody(
            SolidFactory::instance().dynamics_world(), shape, position, mass);

        // Rotate 90 degrees
        btQuaternion quat;
        quat.setRotation(btVector3(1, 0, 0), PI/2.0);
        btTransform trans(quat);
        body->setCenterOfMassTransform(trans);

        // Create GraphicItem object
        ci::gl::VboMesh vbo_mesh(mesh);
        GraphicItem* item = new VboGraphicItem(vbo_mesh, scale);

        SolidPtr solid(new RigidSolid(item, body,
            SolidFactory::instance().dynamics_world()));

        return solid;
    }

    SolidPtr SolidFactory::create_plane(ci::Vec3f dimension,
        ci::Vec3f position) {
        // make a ground plane that cannot be moved
        btCollisionShape * groundShape	= new btStaticPlaneShape(
            btVector3(0,1,0),1);
	    
        btDefaultMotionState * groundMotionState = 
            new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),
            btVector3(0,-1,0)));
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
            0,groundMotionState,groundShape,btVector3(0,0,0));
	
        btRigidBody* body = new btRigidBody(groundRigidBodyCI);
        SolidFactory::instance().dynamics_world()->addRigidBody(body);

        GraphicItem* item = new PlaneGraphicItem(dimension);

        SolidPtr solid(new RigidSolid(item, body, 
            SolidFactory::instance().dynamics_world()));

        return solid;
    }

    SolidPtr SolidFactory::create_static_solid_box(ci::Vec3f dimensions,
        ci::Vec3f position) {
        btCollisionShape* box = new btBoxShape(
            ci::bullet::toBulletVector3(dimensions) / 2.0f);

		btDefaultMotionState* motionState = 
            new btDefaultMotionState(
            btTransform(ci::bullet::toBulletQuaternion(ci::Quatf()),
            ci::bullet::toBulletVector3(position)));
		
		btVector3 inertia(0,0,0);
		float mass = 0.0f; // objects of mass 0 do not move
		box->calculateLocalInertia(mass, inertia);
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, 
            box, inertia);
		
		btRigidBody *rigid_body = new btRigidBody(rigidBodyCI);
		SolidFactory::instance().dynamics_world()->addRigidBody(rigid_body);

        GraphicItem* item = new BoxGraphicItem(dimensions);

        SolidPtr solid(new RigidSolid(item, rigid_body, 
            SolidFactory::instance().dynamics_world()));

        return solid;
    }

    // creates a soft sphere that tries to maintain a constant volume
    SolidPtr SolidFactory::create_soft_sphere(ci::Vec3f position, ci::Vec3f scale) {
        btSoftBody*	soft_body = btSoftBodyHelpers::CreateEllipsoid(
            SolidFactory::instance().soft_body_world_info(),
            btVector3(0, 25, 0), btVector3(1, 1, 1)*3, 20);
        soft_body->m_materials[0]->m_kLST = 0.45;
        soft_body->m_cfg.kVC = 20;
        soft_body->setTotalMass(50,true);
        soft_body->setPose(true,false);

        // change these for different collision types (with other soft, with ridgid, with static...)
        soft_body->m_cfg.collisions |= btSoftBody::fCollision::VF_SS;
        //soft_body->m_cfg.collisions |= btSoftBody::fCollision::SDF_RS;
        //soft_body->m_cfg.collisions |= btSoftBody::fCollision::RVSmask;
        //soft_body->randomizeConstraints();

        SolidFactory::instance().soft_dynamics_world()->addSoftBody(soft_body);

        SolidPtr solid(new SoftSolid(NULL, soft_body, 
            SolidFactory::instance().dynamics_world()));

        return solid;
    }

    SolidPtr SolidFactory::create_sphere_container() {
        ci::app::console() << "creating sphere container" << std::endl;

        ci::ObjLoader loader(ci::loadFileStream("/projects/inc/sock4.obj"));
        ci::TriMesh mesh;
        loader.load(&mesh, true);

        /*
        btBvhTriangleMeshShape* tri_mesh = 
            ci::bullet::createStaticConcaveMeshShape(mesh, ci::Vec3f::one() * 10.0f);
            */

        //ci::app::console() << mesh.getNumIndices() << " : " << mesh.getNumTriangles() << std::endl;

        /*
        btScalar* vertices = new float[mesh.getNumVertices() * 3];

        int i = 0;
        for (std::vector<ci::Vec3f>::const_iterator it = mesh.getVertices().begin();
            it != mesh.getVertices().end(); ++it) { 
            vertices[i] = it->x; ++i;
            vertices[i] = it->y; ++i;
            vertices[i] = it->z; ++i;
        }

        int* triangles = new int[mesh.getNumIndices()];

        i = 0;
        for (std::vector<size_t>::const_iterator it = mesh.getIndices().begin();
            it != mesh.getIndices().end(); ++it) {
            triangles[i] = *it;
            ++i;
        }

        btSoftBody* soft_body = btSoftBodyHelpers::CreateFromTriMesh(
            SolidFactory::instance().soft_body_world_info(),
            vertices, triangles, mesh.getNumTriangles(), false);
        */

        std::vector<ci::Vec3f> vertices = mesh.getVertices();
		std::vector<size_t> indices = mesh.getIndices();
		
		btTriangleMesh* tmesh = new btTriangleMesh(true, false);
		
        for(int i = 0; i < mesh.getNumIndices(); i += 3)
		{
			tmesh->addTriangle(ci::bullet::toBulletVector3(vertices[indices[i]]), 
								ci::bullet::toBulletVector3(vertices[indices[i+1]]), 
								ci::bullet::toBulletVector3(vertices[indices[i+2]]), 
								true);
		}

		btBvhTriangleMeshShape* tri_mesh = new btBvhTriangleMeshShape(tmesh, true, true);
        tri_mesh->setLocalScaling(ci::bullet::toBulletVector3(ci::Vec3f::one() * 10.0f));
		tri_mesh->setMargin(0.05f);

        btDefaultMotionState *motion_state = new btDefaultMotionState(btTransform(
            ci::bullet::toBulletQuaternion(ci::Quatf(-M_PI / 2.0f, 0.0f, 0.0f)),
            ci::bullet::toBulletVector3((ci::Vec3f(0.0, 5.0f, 0.0f)))));
		btRigidBody::btRigidBodyConstructionInfo body_ci(0.0f, motion_state, tri_mesh, btVector3(0,0,0));
		btRigidBody* rigid_body = new btRigidBody(body_ci);

        /*
        btRigidBody* rigid_body = ci::bullet::createStaticRigidBody(SolidFactory::instance().dynamics_world(),
            tri_mesh, ci::Vec3f(0.0f, 50.0f, 0.0f));
        */

        /*
        btMatrix3x3 m;
        // This sets the axis, I think
        m.setEulerZYX(-M_PI / 2.0f, 0.0, 0.0);
        // This sets the origin / starting position
        rigid_body->transform(btTransform(m, 
            ci::bullet::toBulletVector3(ci::Vec3f(0.0f, 50.0f, 0.0f))));
        rigid_body->scale(ci::bullet::toBulletVector3(ci::Vec3f(1.0f, 1.0f, 1.0f)*3.0f));
        */
        
        //SolidFactory::instance().soft_dynamics_world()->addSoftBody(soft_body);
        SolidFactory::instance().soft_dynamics_world()->addRigidBody(rigid_body);

        // TODO: anchor the top points in place

        /*
        SolidPtr solid(new SoftSolid(NULL, soft_solid, 
            SolidFactory::instance().dynamics_world()));
        */

        SolidPtr solid(new RigidSolid(NULL, rigid_body,
            SolidFactory::instance().dynamics_world()));

        /*
        delete [] triangles;
        delete [] vertices;
        */

        return solid;
    }

    btDynamicsWorld* SolidFactory::dynamics_world() {
        return dynamics_world_;
    }

    btSoftRigidDynamicsWorld* SolidFactory::soft_dynamics_world() {
        return dynamics_world_;
    }

    btSoftBodyWorldInfo& SolidFactory::soft_body_world_info() {
        return soft_body_world_info_;
    }

    SolidFactory* SolidFactory::instance_;

    SolidFactory& SolidFactory::instance() {
        return *instance_;
    }

    void SolidFactory::update_object_gravity() {
        SolidList& solid_list = Manager::instance().solids();
        for (SolidList::const_iterator it =  solid_list.begin();
            it != solid_list.end(); ++it) {
            (*it)->set_gravity(gravity_);
        }
    }



    DebugDraw::DebugDraw() : mode_(DBG_NoDebug) {
    }

    void DebugDraw::drawLine(const btVector3& from, const btVector3& to, 
        const btVector3& color) {
        glColor4f(color.x(), color.y(), color.z(), 0.9f);
        glVertex3f(from.x(), from.y(), from.z());
        glVertex3f(to.x(), to.y(), to.z());
    }

    void DebugDraw::drawContactPoint(const btVector3& PointOnB, 
        const btVector3& normalOnB, 
        btScalar distance, int lifeTime, const btVector3& color) {
        // Nothing here
    }

    void DebugDraw::reportErrorWarning(const char* text) {
        ci::app::console() << text << std::endl;
    }

    void DebugDraw::draw3dText(const btVector3& location, const char* text) {
        // Nothing here
    }

    void DebugDraw::setDebugMode(int mode) {
        mode_ = mode;
    }

    int DebugDraw::getDebugMode() const { 
        return mode_;
    }


}