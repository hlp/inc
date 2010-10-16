
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

#include <inc/inc_GraphicItem.h>
#include <inc/inc_LinkFactory.h>
#include <inc/inc_Solid.h>

namespace inc {

class LinkMesh : public GraphicItem {
public:
    LinkMesh(int w, int d, LinkFactory::LinkType,
        std::tr1::shared_ptr<std::deque<SolidPtr> > solids);

    /*
    static std::tr1::shared_ptr<LinkMesh> create_link_mesh(int w, int d,
        LinkFactory::LinkType);
    */

    virtual void draw();

private:
    std::deque<SolidPtr> solids_;
};

}