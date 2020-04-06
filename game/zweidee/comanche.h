#pragma once

#include "proj_render.h"

//#include "2dframebuf.hpp"
#include "zweidee.h"

#include "game.h"

namespace comanche
{
  class CComanche : public game::CGame
  {
    int            heightmap[64][64];
    zweidee::colrgb colormap[64][64];
    void DrawVerticalLine(unsigned char * data, int x, int y1, int y2, zweidee::colrgb col);
  public:
    int render(unsigned char * data, zweidee::point p, int height, int horizon, int scale_height, int distance, int screen_width, int screen_height);
    int init();
    int doit(unsigned char * data);
    int left();
    int right();
    int fire();
  };
}
