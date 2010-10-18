
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

#include <memory>
#include <deque>

#include <inc/inc_GraphicItem.h>
#include <inc/inc_LinkFactory.h>
#include <inc/inc_Solid.h>

class btTypedConstraint;
class btHingeConstraint;
class btPoint2PointConstraint;

namespace inc {

class Joint {
public:
    // this should be pure virtual 
    virtual ci::Vec3f a_position() = 0;
    virtual ci::Vec3f b_position() = 0;
    virtual ci::Vec3f position() = 0;
    virtual btTypedConstraint* constraint_ptr() = 0;

    virtual ~Joint() { }
};

typedef std::tr1::shared_ptr<Joint> JointPtr;

class HingeJoint : public Joint {
public:
    HingeJoint(btHingeConstraint* hinge)
        : hinge_(hinge) { }

    ci::Vec3f a_position();
    ci::Vec3f b_position();
    ci::Vec3f position();

    btTypedConstraint* constraint_ptr() { return hinge_; }

private:
    btHingeConstraint* hinge_;
};

typedef std::tr1::shared_ptr<HingeJoint> HingeJointPtr;

class SocketJoint : public Joint {
public:
    SocketJoint(btPoint2PointConstraint* socket) 
        : socket_(socket) { }

    ci::Vec3f a_position();
    ci::Vec3f b_position();
    ci::Vec3f position();

    btTypedConstraint* constraint_ptr() { return socket_; }

private:
    btPoint2PointConstraint* socket_;
};

typedef std::tr1::shared_ptr<SocketJoint> SocketJointPtr;

class Exporter;

class LinkMesh : public GraphicItem {
public:
    LinkMesh(int w, int d, LinkFactory::LinkType,
        std::tr1::shared_ptr<std::deque<RigidSolidPtr>> solids);
    virtual ~LinkMesh();

    static std::tr1::shared_ptr<LinkMesh> create_link_mesh(int w, int d,
        float radius, float spacing_scale, LinkFactory::LinkType);

    virtual void draw();

    // Override
    void save(Exporter&);

    static int new_mesh_w_;
    static int new_mesh_d_;
    static float new_mesh_height_;
    static int num_lock_points_;
    static ci::Vec3f hinge_axis_;

private:
    std::vector<RigidSolidPtr> solids_;
    std::tr1::shared_ptr<std::vector<JointPtr>> joints_;
    int w_;
    int d_;
};

}