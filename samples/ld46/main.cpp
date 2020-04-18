//////////////////////////////
// main.cpp
// - random noise
//////////////////////////////

#include "stdafx.h"

// buffer dimension - define before include zweidee.h!
#define FBUF2D_WIDTH  64
#define FBUF2D_HEIGHT 48

#include "zweidee.h"
#include <time.h> // rand()

/////////////////////
// put your code here
/////////////////////

int lvl_w = 60; // 32
int lvl_h = 44; // 24
int lvl_size = lvl_w*lvl_h;
char * lvl;

struct rect {
  int l;
  int r;
  int t;
  int b;
}; 

zweidee::point prep_lvl3(zweidee::point seed)
{
  int w = 5 + rand() % (lvl_w - 20);
  int h = 5 + rand() % (lvl_h - 20);
  rect r = {
    __max(0,seed.x - w / 2),
    __min(lvl_h,seed.x + w / 2),
    __max(0,seed.y - h / 2),
    __min(lvl_h,seed.y + h / 2)
  };
  int _gray = rand() % 4;
  for (int y = r.t; y < r.b; y++)
  {
    for (int x = r.l; x < r.r; x++)
    {
      lvl[y*lvl_w + x] = 125;
    }
  }
  int side = rand() % 4;
  int xhalf = r.l + (r.r - r.l) / 2;
  int yhalf = r.t + (r.b - r.t) / 2;
  switch (side) {
  case 0: return { r.l, yhalf};
  case 1: return { r.r, yhalf };
  case 2: return { r.t, xhalf };
  case 3: return { r.b, xhalf };
  }
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  zweidee::app_init(640, 480, hInstance, hPrevInstance, lpCmdLine, nCmdShow);
// initialize rand() once at progstart to reproduce
//  srand(0);
  srand((unsigned int)time(NULL));

  // background / frame
  for (int i = 0; i < FBUF2D_PIXELS; i++)
  {
    int x = i % FBUF2D_WIDTH;
    int y = i / FBUF2D_WIDTH;
    zweidee::fbuf2d.setpixel(zweidee::data, x, y, 111, 111, 111);
  }
  // level
  lvl = new char[lvl_size];
  for (int i = 0; i < lvl_size; i++)
  {
    lvl[i] = 50+rand()%30;
  }
  zweidee::point pt = { lvl_w / 2,lvl_h / 2 };
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  pt = prep_lvl3(pt);
  
  for (int i = 0; i < lvl_size; i++)
  {
    int x = i % lvl_w;
    int y = i / lvl_w;
    zweidee::fbuf2d.setpixel(zweidee::data, (FBUF2D_WIDTH-lvl_w)/2 + x, (FBUF2D_HEIGHT - lvl_h) / 2 + + y, lvl[i], lvl[i], lvl[i]);
  }
  ////////////////
  // do not :-) run in thread 
  ////////////////

  zweidee::repeat = NULL; // 2do: initialize in zweidee.h (need some class)

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);   // hier gibt es Schwierigkeiten mit .bmp,
  zweidee::m_render.Render();

  delete lvl;

  return zweidee::app_run(hInstance);
}
