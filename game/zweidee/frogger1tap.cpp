#include "stdafx.h"

#include "frogger1tap.h"
#include "img_bitmap.hpp"
#include <time.h>

void frogger1tap::CFrog::init()
{
  x = 30;
  y_dummy = FROG_Y_INIT;
  state = start;
}

int frogger1tap::CFrogger::init()
{
  frog.init();
  
  rot = N_ZEILEN-1;

  srand(time(NULL)); // initialize random seed

  for (int z = 0; z < N_ZEILEN; z++)
  {
    int x = 0;
    for (int o = 0; o < N_OBSTACLES; o++) // pro Zeile
    {
      obst[z][o].type = (obst_type)(rand() % 2);
      obst[z][o].x = x + (rand() % 10);
      obst[z][o].w = 10 + rand() % 10;
      obst[z][o].r = obst[z][o].x + obst[z][o].w;
      x = obst[z][o].r + 4;
    }
  }
  return 0;
}

void frogger1tap::CFrogger::draw_obst(unsigned char * data)
{
  for (int z = 0; z < N_ZEILEN; z++)
  {
    int iobst = (rot+z)% N_ZEILEN;
    for (int o = 0; o < N_OBSTACLES; o++)
    {
      int xstart = obst[iobst][o].x;
      int xend = obst[iobst][o].w;
      int y = 5 + z*5;
      char r, g, b;
      if (obst[iobst][o].type == croco) { r = 0; g = 255; b = 0; }
      if (obst[iobst][o].type == tree) { r = 155; g = 125; b = 0; }
      for (int x = xstart; x < (xstart+xend); x++)
      {
        if ((x >= 0) && (x < fbuf2d.width))
//        if (x >= 0)
        {
          int _x = x;// % fbuf2d.width;
          fbuf2d.setPixel(data, _x, y, r, g, b);
          fbuf2d.setPixel(data, _x, y + 1, r, g, b);
        }
      }
    }
  }
}

// ---------------
// main game cycle
// ---------------
int frogger1tap::CFrogger::doit(unsigned char * data)
{
  draw_obst(data);

  // draw frog
  int x = frog.x;
  int y = frog.y_dummy;

  char r, g, b;
  r = g = b = 255;
  fbuf2d.setPixel(data, x, y, r, g, b);
  fbuf2d.setPixel(data, x, y + 1, r, g, b);
  fbuf2d.setPixel(data, x+1, y, r, g, b);
  fbuf2d.setPixel(data, x+1, y+1, r, g, b);

  // move obstacles
/*  if ((fbuf2d.framecounter % 50) == 0)
  {
    if (rot == 0)
      rot = N_ZEILEN - 1;
    else
      rot--;
  }*/
#define SPEED_MAX 10
  int speed = 6; // 1,2,3,4
  bool on_water = true;
  if ((fbuf2d.framecounter % (SPEED_MAX - speed)) == 0)
  {
    for (int z = 0; z < N_ZEILEN; z++)
    {
      int iobst = (rot + z) % N_ZEILEN;
      int y = 5 + z * 5;
      int xmov = (iobst % 2) ? 1 : -1;
      for (int o = 0; o < N_OBSTACLES; o++)
      {
        if (y == frog.y_dummy)
        {
          if ((frog.x >= obst[iobst][o].x) &&
              (frog.x <= obst[iobst][o].r))
          {
            frog.x += xmov;
            on_water = false;
          }
        }
        obst[iobst][o].x += xmov;

        obst[iobst][o].r = obst[iobst][o].x + obst[iobst][o].w; // update r
        if (obst[iobst][o].r == 0) obst[iobst][o].x = fbuf2d.width-1;
        if (obst[iobst][o].x == fbuf2d.width) obst[iobst][o].x = 1;
        obst[iobst][o].r = obst[iobst][o].x + obst[iobst][o].w; // update r again
      }
    }

    // ins Wasser gefallen: sterben
    if ((frog.state == floating) && (on_water)) frog.init();
    // am Spielfeldrand: sterben (es sei denn ein magic item)
    if (frog.x == 0) frog.init();
    if (frog.x == fbuf2d.width) frog.init();
  }

  return 0;
}

int frogger1tap::CFrogger::fire() // == Tap()
{
  if ((fbuf2d.framecounter % 4) == 0)
  {
    if (frog.y_dummy == FROG_Y_INIT)
    {
      // init
      frog.y_dummy -= 5;
//      if (frog.y_dummy == -5) frog.y_dummy = FROG_Y_INIT;
    }
    else
    {
      // we're floating
      frog.state = floating;
      if (rot == 0)
        rot = N_ZEILEN - 1;
      else
        rot--;
    }
  }
  return 0;
}

int frogger1tap::CFrogger::left() // virtual function, needs to be implemented, otherwise class is abstract
{
  return 0;
}

int frogger1tap::CFrogger::right() // virtual function, needs to be implemented, otherwise class is abstract
{
  return 0;
}
