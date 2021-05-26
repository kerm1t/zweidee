#pragma once

//#include "zweidee.h"
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
#ifdef LD46
#include "../samples/ld46/ld46.h"
#endif
#ifdef GLENZ
#include "../samples/glenz/glenz.h"
#endif
namespace zweidee
{

  class CEngine // formerly "proj"
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
#ifdef LD46
    ld46::CLD46 m_game; // ugly!!.. 2do, das muss hier 'raus
#endif
#ifdef GLENZ
    glenz::CGlenz m_game; // ugly!!.. 2do, das muss hier 'raus
#endif
//    zweidee::FrameBuf2D * fbuf2d;
//    unsigned char * data;

  public:
    bool bPause;
    bool bStep;

    // Constructor

    CEngine();
    ~CEngine();

    // Variables
    zweidee::FrameBuf2D * fbuf2d;
    unsigned char * data;

//    zweidee::Render m_render;

    // Functions
    
    int init(FrameBuf2D * fb2, unsigned char * dta);
    int move();
    int render();

    int fire();
    int up();
    int down();
    int left();
    int right();
  };
}
