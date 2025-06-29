#pragma once

#include "zweidee.h"
#include "game.h"

namespace glenz
{
  class CGlenz : public game::CGame
  {
    void lesson2(unsigned char * data); // draw 3 filled triangles
  public:
    int init();
    int doit(unsigned char * data);
    int left();
    int right();
    int fire();
    int up();
    int down();
  };
}
