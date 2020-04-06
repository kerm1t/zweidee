#pragma once

//#include "obj.hpp"

#include "proj_render.h"

#include "2dframebuf.hpp"
#include "../game/zweidee/galaga.h"
#include "../game/zweidee/pitstop.h"
#include "../game/zweidee/comanche.h"
#include "../game/zweidee/frogger1tap.h"

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

    zweidee::Render m_render;

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
    galaga::CGalaga   m_game;
//    pitstop::CPitstop   m_game;
//    comanche::CComanche m_game;
//    frogger1tap::CFrogger m_game;

    FrameBuf2D    * fbuf2d;
    unsigned char * data;

    /* ===== FUNCTIONS ===== */
    
//    int draw_arrow(unsigned char * data);
//    int draw_progress(unsigned char * data);
  };
}
