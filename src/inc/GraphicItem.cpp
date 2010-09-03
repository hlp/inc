
#include <cinder/gl/gl.h>
#include <cinder/Vector.h>

#include <inc/GraphicItem.h>

namespace inc {

    BoxGraphicItem::BoxGraphicItem(ci::Vec3f dims) : dimensions_(dims) {
    }

    void BoxGraphicItem::draw() {
        glColor4f(0.0f, 1.0f, 1.0f, 0.9f);
        glLineWidth(0.9f);
        ci::gl::enableWireframe();
        ci::gl::drawCube(ci::Vec3f::zero(), dimensions_);
    }


    PlaneGraphicItem::PlaneGraphicItem(ci::Vec3f dims) : dimensions_(dims) {
    }

    void PlaneGraphicItem::draw() {
        ci::gl::drawCube( ci::Vec3f::zero(), 
            ci::Vec3f(dimensions_.x, 0.1f, dimensions_.z) );        
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
        glColor4f( 1.0f, 1.0f, 1.0f, 0.55f );
        glLineWidth(3.0f);
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
        glColor4f( 1.0f, 0.0f, 0.0f, 0.75f );
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
}