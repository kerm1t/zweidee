#pragma once

#include "zweidee.h"

namespace game
{
  typedef struct
  {
    int x; // left
    int y; // top
    int w;
    int h;
  } rect;

  typedef struct
  {
    rect box;
  } sPoint;

  typedef struct
  {
    bool on;
    rect box;
  } sShot;

  class CGame
  {
  public:
//    zweidee::FrameBuf2D fbuf2d;
    zweidee::FrameBuf2D * fbuf2d;

#define NUM_EPISODES 3
    char a_episode_len[NUM_EPISODES];
    bool a_episode_active[NUM_EPISODES];

    unsigned int iepisode;
    int iloopy;

    // virtual functions have to be all redefined by derived class
    virtual int init() = 0;
    virtual int doit(unsigned char * data) = 0;
    // keys
    virtual int left() = 0;
    virtual int right() = 0;
    virtual int fire() = 0;
    virtual int up() = 0;
    virtual int down() = 0;

    // https://gamedev.stackexchange.com/questions/586/what-is-the-fastest-way-to-work-out-2d-bounding-box-intersection
    /*bool galaga::DoBoxesIntersect(rect a, rect b) // for center + width / height
    {
    // translate (top,left,w,h) to (x,y,w,h) <--- optimize !!!
    int ax = a.x + (int)(a.w/2);
    int ay = a.y + (int)(a.h/2);
    int bx = b.x + (int)(b.w/2);
    int by = b.y + (int)(b.h/2);
    return (abs(ax - bx) * 2 < (a.w + b.w)) &&
    (abs(ay - by) * 2 < (a.h + b.h));
    }
    */
    bool DoBoxesIntersect(rect a, rect b) // for top, left, "right", "bottom"
    {
      return !(b.x      > (a.x + a.w)
        || (b.x + b.w) <  a.x
        || b.y      > (a.y + a.h)
        || (b.y + b.h) <  a.y);
    }
  };
}
