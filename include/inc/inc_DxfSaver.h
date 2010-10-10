
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

#include <string>
#include <fstream>
#include <vector>

#include <cinder/gl/gl.h>
#include <cinder/Vector.h>

namespace inc {

class SoftSolid;

class Exporter {
public:
    virtual ~Exporter() { }

    virtual void input_soft_solid(SoftSolid& solid) = 0;

    virtual void set_layer(int) = 0;
    virtual void add_layer() = 0;
};


class DxfSaver : public Exporter {
public:
    DxfSaver(const std::string& file_name);

    void set_path(const std::string&);
    
    void input_soft_solid(SoftSolid& solid);

    void begin();
    void end();

    void set_layer(int);
    void add_layer();

    //void write_file();

private:
    void write_header();
    void write_footer();
    void write(std::string command, float value);
    void write_line(const ci::Vec3f& v1, const ci::Vec3f& v2);
    void write_triangle(float v1x, float v1y, float v1z,
        float v2x, float v2y, float v2z,
        float v3x, float v3y, float v3z);
    void write_triangle(const ci::Vec3f& v1,
        const ci::Vec3f& v2, const ci::Vec3f& v3);

    std::ofstream file_;
    std::string file_name_;

    std::vector<ci::Vec3f> vertices_;
    int current_layer_;

    float INC_EPSILON;
};

}