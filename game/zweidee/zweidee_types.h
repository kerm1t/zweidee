#pragma once

namespace zweidee
{

  struct colrgb
  {
    char r;
    char g;
    char b;
  };

  struct point
  {
    unsigned int x;
    unsigned int y;
    point(const unsigned int _x, const unsigned int _y) : x(_x), y(_y) {}
  };

}
