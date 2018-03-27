#include "stdafx.h"

#include "frogger1tap.h"
#include "img_bitmap.hpp"
#include <time.h>
//#include <algorithm>

void frogger1tap::CFrog::init()
{
  state = start;

  // start unten
//  x = FBUF2D_WIDTH / 2;
//  y_dummy = FROG_Y_INIT;
  lane = 8;

  // start seitlich
  x = -23; // outside of area (-) centered within "start area"
  y_dummy = (int)PLAYFIELD_H / 2;
}

int frogger1tap::CFrogger::init()
{
  frog.init();
  
  rot = N_ZEILEN-1;

  srand(time(NULL)); // initialize random seed

  int obstwidthconst = FBUF2D_WIDTH / 6;

  for (int z = 0; z < N_ZEILEN; z++)
  {
    int x = 0;
    for (int o = 0; o < N_OBSTACLES; o++) // pro Zeile
    {
      obst[z][o].type = (obst_type)(rand() % 2);
      obst[z][o].x = x + (rand() % obstwidthconst);
      obst[z][o].w = obstwidthconst + rand() % obstwidthconst;
      obst[z][o].r = obst[z][o].x + obst[z][o].w;
//      obst[z][o].r = std::min(obst[z][o].x, FBUF2D_WIDTH);
      x = obst[z][o].r + LANE_HEIGHT;
    }
  }

  ystart = AREA_HEAD;

  return 0;
}

void frogger1tap::CFrogger::draw_obst(unsigned char * data)
{
  for (int z = 0; z < N_ZEILEN; z++)
  {
    int iobst = (rot+z)% N_ZEILEN;
#define OBST_MODULO_Y AREA_FLOATING
    int y = (ystart + z*LANE_HEIGHT) % OBST_MODULO_Y;

    // -------------
    // move frog !!!
    // -------------
    if ((frog.state == floating) && (z == frog.lane))
    {
      frog.y_dummy = y;
    }
    // -------------
    // move frog !!!
    // -------------

    for (int o = 0; o < N_OBSTACLES; o++)
    {
      int xstart = obst[iobst][o].x;
      int xend = xstart + obst[iobst][o].w;
      char r, g, b;
      if (obst[iobst][o].type == croco) { r = 0; g = 255; b = 0; }
      if (obst[iobst][o].type == tree) { r = 155; g = 125; b = 0; }

      for (int x = xstart; x < xend; x++)
      {
        if (x % LANE_HEIGHT == 0) continue; // graph. trick (schwarze Linien)
        if (x % LANE_HEIGHT == 1) continue; // graph. trick (schwarze Linien)
        int _x = x % (AREA_W - 1);
        for (int yinc = 0; yinc < OBJ_HEIGHT; yinc++)
        {
          fbuf2d.setPixel(data, PLAYFIELD_X_MIN + _x, y+yinc, r, g, b);
        }
      }
    }
  }
}

void frogger1tap::CFrogger::draw_frog(unsigned char * data)
{
  int xs = frog.x;
  int ys = frog.y_dummy;

  char r, g, b;
  r = g = b = 255;
  for (int y = 0; y < OBJ_HEIGHT; y++)
  {
    for (int x = 0; x < OBJ_HEIGHT; x++) // quadratic ;-)
    {
      fbuf2d.setPixel(data, PLAYFIELD_X_MIN + xs+x, ys+y, r, g, b);
    }
  }
}

void frogger1tap::CFrogger::draw_area_start(unsigned char * data)
{
  for (int y = 0; y < PLAYFIELD_H; y++)
  {
    for (int x = 0; x < PLAYFIELD_X_MIN; x++) // pro Zeile
    {
      fbuf2d.setPixel(data, x, y, 100, 100, 100);
    }
  }
}

// ---------------
// main game cycle
// ---------------
int frogger1tap::CFrogger::doit(unsigned char * data)
{
  draw_obst(data);


  draw_area_start(data); // move to init later
  draw_frog(data);


  // move obstacles
/*  if ((fbuf2d.framecounter % 50) == 0)
  {
    if (rot == 0)
      rot = N_ZEILEN - 1;
    else
      rot--;
  }*/
  // rot (die lanes werden als Ganzes rotiert) ist
  // ersetzt durch ystart (das Spielfeld wird pixelweise gescrollt)
#define SPEED_VERT_MAX 10
  int speed_vert = 5; // 1,2,3,4
  if ((fbuf2d.framecounter % (SPEED_VERT_MAX - speed_vert)) == 0)
  {
    ystart++;
    if (ystart == PLAYFIELD_H) ystart = 0;
  }

#define SPEED_HORIZ_MAX 10
  int speed_horiz = 8; // 1,2,3,4
  bool on_water = true;
  if ((fbuf2d.framecounter % (SPEED_HORIZ_MAX - speed_horiz)) == 0)
  {
    for (int z = 0; z < N_ZEILEN; z++)
    {
      int iobst = (rot + z) % N_ZEILEN;
      int y = ystart + z * LANE_HEIGHT;
      int xmov = (iobst % 2) ? 1 : -1;
      for (int o = 0; o < N_OBSTACLES; o++)
      {
        if (z == frog.lane)
        {
          if ((frog.x >= obst[iobst][o].x-5) &&  // fangbereich +/-5
              (frog.x+OBJ_HEIGHT <= obst[iobst][o].r+5))
          {
            frog.x += xmov;
            on_water = false;
          }
        }
        obst[iobst][o].x += xmov;

        if (obst[iobst][o].x == 0) obst[iobst][o].x = AREA_W-1;
        if (obst[iobst][o].x == AREA_W) obst[iobst][o].x = 1;
        obst[iobst][o].r = obst[iobst][o].x + obst[iobst][o].w; // update r
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

int frogger1tap::CFrogger::fire() // == Tap()      <-- Einzelfeuer, s. zweidee.cpp, WndProc()
{
//  if ((fbuf2d.framecounter % 4) == 0)
  {
    if (frog.state == start)
    {
      // jump into area!
      frog.state = floating;
      frog.x = 80;
      // estimate lane
      for (int z = 0; z < N_ZEILEN; z++)
      {
        int y = (ystart + z*LANE_HEIGHT) % OBST_MODULO_Y;
        if (abs(y - frog.y_dummy) < 10) { frog.lane = z; break; }
      }
//      frog.lane = 8;
    }
    else
    {
      // we're floating
      if (frog.lane == 0) frog.lane = N_ZEILEN-1;
      else frog.lane--;
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
