
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

#include <BulletSoftBody/btSoftBody.h>

#include <cinder/gl/gl.h>
#include <cinder/Vector.h>
#include <cinder/app/App.h>
#include <cinder/CinderMath.h>

#include <inc/inc_GraphicItem.h>
#include <inc/inc_Solid.h>
#include <inc/inc_Renderer.h>
#include <inc/inc_Color.h>

namespace inc {

GraphicItem::GraphicItem() {
    visible_ = true;
}

GraphicItem::~GraphicItem() {
#ifdef TRACE_DTORS
    ci::app::console() << "Deleting GraphicItem" << std::endl;
#endif
}

void GraphicItem::set_visible(bool vis) {
    visible_ = vis;
}

bool GraphicItem::visible() {
    return visible_;
}

SolidGraphicItem::~SolidGraphicItem() { 
}

Solid& SolidGraphicItem::solid() {
    return *solid_;
}

void SolidGraphicItem::set_solid(Solid* s) {
    solid_ = s;
}


OriginGraphicItem::OriginGraphicItem() {
    grid_plane_size_ = 1000.0f;
    grid_plane_intervals_ = 30;
}

void OriginGraphicItem::draw() {
    draw_axis();
    draw_grid_plane_lines();
}

void OriginGraphicItem::draw_axis() {
    glColor4f( 1.0f, 1.0f, 1.0f, 0.25f );
    glLineWidth(2.0f);
#if ! defined( CINDER_GLES )
    glEnable( GL_LINE_STIPPLE );
    glLineStipple( 10, 0xAAAA );
#endif

    float line_verts[3*6];
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, line_verts );
    line_verts[0] = -1000.0f; line_verts[1] = 0.0f; line_verts[2] = 0.0f;
    line_verts[3] = 1000.0f; line_verts[4] = 0.0f; line_verts[5] = 0.0f; 
    line_verts[6] = 0.0f; line_verts[7] = -1000.0f; line_verts[8] = 0.0f;
    line_verts[9] = 0.0f; line_verts[10] = 1000.0f; line_verts[11] = 0.0f;
    line_verts[12] = 0.0f; line_verts[13] = 0.0f; line_verts[14] = -1000.0f;
    line_verts[15] = 0.0f; line_verts[16] = 0.0f; line_verts[17] = 1000.0f; 
    glDrawArrays( GL_LINES, 0, 6 );
    glDisableClientState( GL_VERTEX_ARRAY );

#if ! defined( CINDER_GLES )
    glDisable( GL_LINE_STIPPLE );
#endif
}

void OriginGraphicItem::draw_grid_plane_lines() {
    glColor4f( .9019f, 0.4039f, 0.0f, 0.25f );
    glLineWidth(0.5f);

    int num_lines = 2*grid_plane_intervals_*4;
    float* line_verts = new float[3*num_lines];
    //float line_verts[];
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer(3, GL_FLOAT, 0, line_verts);

    float grid_step = grid_plane_size_ / grid_plane_intervals_;
    float x = -grid_plane_size_;
    int index = 0;
    for (int c = -grid_plane_intervals_; c < grid_plane_intervals_; ++c) {
        line_verts[index] = x; line_verts[index+1] = 0.0f; line_verts[index+2] =  -grid_plane_size_;
        line_verts[index+3] = x; line_verts[index+4] = 0.0f; line_verts[index+5] = grid_plane_size_;
        index += 6;
        x += grid_step;
    }
    float y = -grid_plane_size_;
    for (int c = -grid_plane_intervals_; c < grid_plane_intervals_; ++c) {
        line_verts[index] = -grid_plane_size_; line_verts[index+1] = 0.0f; line_verts[index+2] = y;
        line_verts[index+3] = grid_plane_size_; line_verts[index+4] = 0.0f; line_verts[index+5] = y;
        index += 6;
        y += grid_step;
    }

    glDrawArrays( GL_LINES, 0, num_lines );
    glDisableClientState( GL_VERTEX_ARRAY );

    delete [] line_verts;
}

float* OriginGraphicItem::grid_plane_size_ptr() {
    return &grid_plane_size_;
}

float* OriginGraphicItem::grid_plane_intervals_ptr() {
    return &grid_plane_intervals_;
}


BoxGraphicItem::BoxGraphicItem(ci::Vec3f dims) : dimensions_(dims) {
}

void BoxGraphicItem::draw() {
    glColor4f(0.0f, 1.0f, 1.0f, 0.25f);
    glLineWidth(3.0f);
    ci::gl::enableWireframe();
    ci::gl::drawCube(ci::Vec3f::zero(), dimensions_);
}


PlaneGraphicItem::PlaneGraphicItem(ci::Vec3f dims) : dimensions_(dims) {
}

void PlaneGraphicItem::draw() {
    ci::gl::drawCube( ci::Vec3f::zero(), 
        ci::Vec3f(dimensions_.x, 0.1f, dimensions_.z) );        
}


VboGraphicItem::VboGraphicItem(ci::gl::VboMesh& mesh, ci::Vec3f scale) {
    vbo_mesh_ = mesh;
    scale_ = scale;
}

void VboGraphicItem::draw() {
    glColor4f(0.0f, 1.0f, 1.0f, 0.9f);
    glLineWidth(0.9f);
    ci::gl::enableWireframe();
    ci::gl::pushMatrices();
        ci::gl::scale(scale_);
        ci::gl::draw(vbo_mesh_);
    ci::gl::popMatrices();
}


SphereGraphicItem::SphereGraphicItem(float r) : radius_(r) {
}

SphereGraphicItem::~SphereGraphicItem() {
}

void SphereGraphicItem::draw() {
    if (solid().selected())
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    else
        glColor4f(1.0f, 1.0f, 1.0f, 0.4);

    glLineWidth(0.9f);
    ci::gl::enableWireframe();
    ci::gl::drawSphere(ci::Vec3f::zero(), radius_);
    ci::gl::disableWireframe();
}

float SphereGraphicItem::bounding_sphere_radius() {
    return radius_;
}



bool SoftBodyGraphicItem::draw_face_normals_;
float SoftBodyGraphicItem::face_normals_length_;
ci::ColorA SoftBodyGraphicItem::face_normals_color_;

SoftBodyGraphicItem::SoftBodyGraphicItem(btSoftBody* soft_body,
    ci::ColorA color) : soft_body_(soft_body), color_(color) {

    last_min_y_ = get_vertex_height(0, 0);
    last_max_y_ = get_vertex_height(0, 0);
}

SoftBodyGraphicItem::~SoftBodyGraphicItem() {
}

// theres almost certainly a better way to implement this method
void SoftBodyGraphicItem::draw() {
    int num_faces = soft_body_->m_faces.size();

    glBegin(GL_TRIANGLES);

    float num_verts = num_faces * 3;
    float v = 0.0f;

    float r1 = Renderer::instance().base_color().r;
    float g1 = Renderer::instance().base_color().g;
    float b1 = Renderer::instance().base_color().b;
    float a1 = Renderer::instance().base_color().a;

    float r2 = Renderer::instance().top_color().r;
    float g2 = Renderer::instance().top_color().g;
    float b2 = Renderer::instance().top_color().b;
    float a2 = Renderer::instance().top_color().a;

    float vert_height;

    float curr_min_y = get_vertex_height(0, 0);
    float curr_max_y = get_vertex_height(0, 0);

    for (int i = 0; i < num_faces; ++i) {
        vert_height = get_vertex_height(i, 0);
        
        Color::set_color_a(
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, r1, r2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, g1, g2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, b1, b2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, a1, a2) );
        
        make_gl_vertex(i, 0);

        vert_height = get_vertex_height(i, 1);

        Color::set_color_a(
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, r1, r2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, g1, g2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, b1, b2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, a1, a2) );

        make_gl_vertex(i, 1);

        vert_height = get_vertex_height(i, 2);

        Color::set_color_a(
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, r1, r2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, g1, g2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, b1, b2),
            ci::lmap<float>(vert_height, last_min_y_, last_max_y_, a1, a2) );

        make_gl_vertex(i, 2);

        if (vert_height >= curr_max_y)
            curr_max_y = vert_height;

        if (vert_height <= curr_min_y)
            curr_min_y = vert_height;
    }

    last_max_y_ = curr_max_y;
    last_min_y_ = curr_min_y;

    glEnd();

    return;

    ci::ColorA line_color;

    if (solid().selected()) {
        line_color = ci::ColorA(1.0f, 1.0f, 0.0f, 1.0f);
    } else {
        line_color = Renderer::instance().line_color();
    }

    Color::set_color_a(line_color);

    glLineWidth(Renderer::instance().line_thickness());

    glBegin(GL_LINES);
    
    for (int i = 0; i < num_faces; ++i) {
        make_gl_vertex(i, 0);
        make_gl_vertex(i, 1);

        make_gl_vertex(i, 1);
        make_gl_vertex(i, 2);

        make_gl_vertex(i, 0);
        make_gl_vertex(i, 2);
    }

    glEnd();

    if (!draw_face_normals_)
        return;

    Color::set_color_a(face_normals_color_);

    glBegin(GL_LINES);

    btVector3 normal_vec;
    ci::Vec3f face_center;

    for (int i = 0; i < num_faces; ++i) {
        normal_vec = soft_body_->m_faces[i].m_normal;
        face_center = get_face_center(i);
        
        glVertex3f(face_center);
        glVertex3f(face_center + ci::Vec3f(normal_vec.x(), normal_vec.y(),
            normal_vec.z()) * face_normals_length_);
    }

    glEnd();
}

bool SoftBodyGraphicItem::detect_selection(ci::Ray r) {
    int num_faces = soft_body_->m_faces.size();

    ci::Vec3f v1, v2, v3;
    float dist;

    for (int i = 0; i < num_faces; ++i) {
        v1 = ci::Vec3f(soft_body_->m_faces[i].m_n[0]->m_x.x(),
            soft_body_->m_faces[i].m_n[0]->m_x.y(),
            soft_body_->m_faces[i].m_n[0]->m_x.z());

        v2 = ci::Vec3f(soft_body_->m_faces[i].m_n[1]->m_x.x(),
            soft_body_->m_faces[i].m_n[1]->m_x.y(),
            soft_body_->m_faces[i].m_n[1]->m_x.z());

        v3 = ci::Vec3f(soft_body_->m_faces[i].m_n[2]->m_x.x(),
            soft_body_->m_faces[i].m_n[2]->m_x.y(),
            soft_body_->m_faces[i].m_n[2]->m_x.z());

        if (r.calcTriangleIntersection(v1, v2, v3, &dist))
            return true;
    }

    return false;
}

}