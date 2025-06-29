#pragma once

//#include "zweidee.h"
#include "glenz.h"

namespace zweidee
{

  class CEngine // formerly "proj"
  {
  private:
    // Variables

    // Generic Variables

    glenz::CGlenz m_game; // ugly!!.. 2do, das muss hier 'raus
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
