//////////////////////////////
// main.cpp
// - random noise
//////////////////////////////

#include "stdafx.h"

// buffer dimension - define before include zweidee.h!
#define FBUF2D_WIDTH  64
#define FBUF2D_HEIGHT 48

#include "zweidee.h"
#include "engine.h"
#include "ld46.h"
#include <time.h> // rand()

/////////////////////
// put your code here
/////////////////////

struct rect {
  int l;
  int r;
  int t;
  int b;
}; 

zweidee::point prep_lvl3(zweidee::point seed)
{
  int w = 5 + rand() % (ld46::lvl_w - 20);
  int h = 5 + rand() % (ld46::lvl_h - 20);
  rect r = {
    __max(0,seed.x - w / 2),
    __min(ld46::lvl_h,seed.x + w / 2),
    __max(0,seed.y - h / 2),
    __min(ld46::lvl_h,seed.y + h / 2)
  };
  int _gray = rand() % 4;
  for (int y = r.t; y < r.b; y++)
  {
    for (int x = r.l; x < r.r; x++)
    {
      ld46::lvl[y*ld46::lvl_w + x] = 125;
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
  return {0,0};
}

zweidee::CEngine m_engine;

void do_stuff_here() // b) function
{
  // no rapid fire :-)
  if (GetAsyncKeyState(VK_UP))    m_engine.up();
  if (GetAsyncKeyState(VK_DOWN))  m_engine.down();
  if (GetAsyncKeyState(VK_LEFT))  m_engine.left();
  if (GetAsyncKeyState(VK_RIGHT)) m_engine.right();

  // draw bg
  for (int i = 0; i < ld46::lvl_size; i++)
  {
    int x = (FBUF2D_WIDTH - ld46::lvl_w) / 2 + i % ld46::lvl_w;
    int y = (FBUF2D_HEIGHT - ld46::lvl_h) / 2 + i / ld46::lvl_w;
    zweidee::fbuf2d.setpixel(zweidee::data, x, y, ld46::lvl[i], ld46::lvl[i], ld46::lvl[i]);
  }

  m_engine.move(); // move (i.e. increase playengine step) and update fbuf2d
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  zweidee::app_init(640, 480, hInstance, hPrevInstance, lpCmdLine, nCmdShow);

  m_engine.init(&zweidee::fbuf2d, zweidee::data);

  // initialize rand() once at progstart to reproduce, use e.g. srand(0) to get to a defined chain;
  srand((unsigned int)time(NULL));

  // background / frame
  for (int i = 0; i < FBUF2D_PIXELS; i++)
  {
    int x = i % FBUF2D_WIDTH;
    int y = i / FBUF2D_WIDTH;
    zweidee::fbuf2d.setpixel(zweidee::data, x, y, 111, 111, 111);
  }
  // level
  ld46::lvl = new char[ld46::lvl_size];
  for (int i = 0; i < ld46::lvl_size; i++)
  {
    ld46::lvl[i] = 50+rand()%30;
  }
  zweidee::point pt = { ld46::lvl_w / 2, ld46::lvl_h / 2 };
  for (int i = 0; i < 11; i++)
  {
    pt = prep_lvl3(pt);
  }
  
  ////////////////
  // do not :-) run in thread 
  ////////////////

  zweidee::repeat = do_stuff_here;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);   // hier gibt es Schwierigkeiten mit .bmp,
  zweidee::m_render.Render();

  int res = zweidee::app_run(hInstance);
  delete ld46::lvl;
  return res;
}
