#pragma once

#include "obj.hpp"

#include "proj_render.h"

#include "2dframebuf.hpp"
#include "../game/zweidee/galaga.h"

namespace proj
{

  class Proj
  {
  public:
    bool bPause;
    bool bStep;

    /* ===== Constructor ===== */

    Proj();
    ~Proj();

    /* ===== Variables ===== */

    proj::Render m_render;

    /* ===== FUNCTIONS ===== */
    
    int init();
    int move();
    int render();

    int fire();
    int up();
    int down();
    int left();
    int right();

  private:
    /* ===== Variables ===== */

    // Generic Variables
    galaga::CGalaga m_galaga;

    FrameBuf2D    * fbuf2d;
    unsigned char * data;

    /* ===== FUNCTIONS ===== */
    
    int draw_arrow(unsigned char * data);
    int draw_progress(unsigned char * data);
  };
}
