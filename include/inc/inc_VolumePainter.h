
#pragma once

#include <inc/inc_Module.h>

namespace inc {

class VolumePainter : public Module {
public:
    VolumePainter();

    void setup();
    void update();
    void draw();

};

}