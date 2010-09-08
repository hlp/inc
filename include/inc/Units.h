
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

namespace inc {

   typedef float Length;
   typedef float SU; // Standard Unit

   SU centimeter_to_standard(Length cm) {
       return cm * 0.01f;
   }

   SU millimeter_to_standard(Length mm) {
       return mm * 0.001f;
   }

   SU meter_to_standard(Length m) {
       return m;
   }

   SU foot_to_standard(Length ft) {
       return ft * 0.3048f;
   }

   SU inch_to_standard(Length in) {
       return in * 0.0254f;
   }
}