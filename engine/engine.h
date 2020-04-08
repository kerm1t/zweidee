#pragma once

#include "zweidee.h"
#ifdef GALAGA
#include "../samples/galaga/galaga.h"
#endif
#ifdef PITSTOP
#include "../samples/pitstop/pitstop.h"
#endif
#ifdef COMANCHE
#include "../samples/comanche/comanche.h"
#endif
#ifdef FROGGER
#include "../samples/frogger/frogger1tap.h"
#endif

namespace zweidee
{

  class Engine // formerly "proj"
  {
  private:
    // Variables

    // Generic Variables
#ifdef GALAGA
    galaga::CGalaga m_game;
#endif
#ifdef PITSTOP
    pitstop::CPitstop m_game;
#endif
#ifdef COMANCHE
    comanche::CComanche m_game;
#endif
#ifdef FROGGER
    frogger1tap::CFrogger m_game;
#endif

    zweidee::FrameBuf2D * fbuf2d;
    unsigned char * data;

  public:
    bool bPause;
    bool bStep;

    // Constructor

    Engine();
    ~Engine();

    // Variables

    zweidee::Render m_render;

    // Functions
    
    int init();
    int move();
    int render();

    int fire();
    int up();
    int down();
    int left();
    int right();
  };
}
