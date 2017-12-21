#pragma once

#include "proj_render.h"

#include "2dframebuf.hpp"

#include "game.h"

namespace comanche
{
  class CComanche : public game::CGame
  {
  public:
    int init();
    int doit(unsigned char * data);
    int left();
    int right();
    int fire();
  };
}
