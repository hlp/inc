
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