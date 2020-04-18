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
enum ecolor {RED,GREEN,BLUE};
zweidee::colrgb color[11] = { { 0,0,0 }, { 255,0,0 },{ 0,225,0 },{ 0,0,255 },{ 255,255,0 },
                              { 0,255,255 },{ 255,0,225 },{ 125,0,0 },{ 0,125,0 },{ 0,0,125 },
                              { 125,125,0 } };
int lvl_w = 32;
int lvl_h = 24;
int lvl_size = lvl_w*lvl_h;
uint8 * lvl;
struct lpoint {//: zweidee::point {
  int x;
  int y;
  bool visited;
//  lpoint() { } // default constructor
//  lpoint(const uint32 _x, const uint32 _y, const bool _v) : x(_x), y(_y), visited(_v) { } // constructor for change after declaration
};
#define LVL_LIMITER 10
float eudistance(zweidee::point p1, zweidee::point p2)
{
  int dx = (int)p2.x - (int)p1.x; // to not subtract uint!!
  int dy = (int)p2.y - (int)p1.y;
  return sqrtf((float)dx*dx + (float)dy*dy);
}
void prep_lvl()
{
  lpoint limiter[LVL_LIMITER];
  for (int i = 0; i < LVL_LIMITER; i++)
  {
    int j = rand() % lvl_size;
    limiter[i] = { j % lvl_w, j / lvl_w, false };
    lvl[j] = 255;// RED;// 255; // just for visualization, may remove later
  }

  // now use these random points as constraints for level walls
  zweidee::point seed = { rand() % lvl_w, rand() % lvl_h };
  lvl[seed.y*lvl_w + seed.x] = 88;// GREEN;// 88;
  // a) expand
  bool hit = false; // actually we need hit-l/r/t/b --> bitfield
  while (!hit)
  {
    // expand OR search 4 nearest points
    // [..]
    hit = true;
  }
  for (int h = 0; h < 4; h++)
  {
    float d = eudistance({ 27,7 }, { 23,11 });
    int idxmin = 255;
    float distmin = 255;
    for (int i = 0; i < LVL_LIMITER; i++)
    {
      if (!limiter[i].visited)
      {
        zweidee::point pt = zweidee::point(limiter[i].x, limiter[i].y);
        float dist = eudistance(seed, pt);
        if (dist < distmin)
        {
          distmin = dist;
          idxmin = i;
        }
      }
    } // for i
    if (idxmin < 255) limiter[idxmin].visited = true;
  } // for h
  for (int i = 0; i < LVL_LIMITER; i++)
  {
    if (limiter[i].visited) {
      lvl[limiter[i].y*lvl_w + limiter[i].x] = 150;// BLUE;// 150; // just for visualization, may remove later
    }
  }
}
struct rect {
  int l;
  int r;
  int t;
  int b;
};
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))
rect prep_lvl2(rect r)
{
  if (((r.r - r.l) == 0) || ((r.b - r.t)==0)) return{ 0,0,0,0 };

  zweidee::point seed = { r.l + rand() % (r.r-r.l), r.t + rand() % (r.b-r.t) };
  int w = rand() % 20;
  int h = rand() % 20;
  rect rtmp = {
    __max(0,seed.x - w / 2),
    __min(lvl_h,seed.x + w / 2),
    __max(0,seed.y - h / 2),
    __min(lvl_h,seed.y + h / 2)
  };
  int _color = rand() % 200;
  for (int y = __max(0,seed.y - h / 2); y < __min(lvl_h,seed.y + h / 2); y++)
  {
    for (int x = __max(0,seed.x - w / 2); x < __min(lvl_h,seed.x + w / 2); x++)
    {
      lvl[y*lvl_w + x] = 55+_color;// BLUE;// 150; // just for visualization, may remove later
    }
  }
  return rtmp;
/*  int side = rand() % 4;
  int tmp;
  switch (side) {
    case 0: tmp = rtmp.l; break;
    case 1: tmp = rtmp.r; break;
    case 2: tmp = rtmp.t; break;
    case 3: tmp = rtmp.b; break;
  }
  return rect{tmp,tmp,tmp,tmp};
  */
}
int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  zweidee::app_init(640, 480, hInstance, hPrevInstance, lpCmdLine, nCmdShow);
// initialize once at program start to reproduce
  srand(0);// (uint32)time(NULL));

  // random noise
  for (int i = 0; i < FBUF2D_PIXELS; i++)
  {
    int x = i % FBUF2D_WIDTH;
    int y = i / FBUF2D_WIDTH;
    zweidee::fbuf2d.setpixel(zweidee::data, x, y, rand(), rand(), rand());
  }

  lvl = new uint8[lvl_size];
  for (int i = 0; i < lvl_size; i++)
  {
    lvl[i] = 0;
  }
  rect r = { 0,lvl_w,0,lvl_h };
  r = prep_lvl2(r);
  r = prep_lvl2(r);
  r = prep_lvl2(r);
  r = prep_lvl2(r);
//  prep_lvl2(r);
  for (int i = 0; i < lvl_size; i++)
  {
    int x = i % lvl_w;
    int y = i / lvl_w;
    zweidee::fbuf2d.setpixel(zweidee::data, 16 + x, 12 + y, lvl[i], lvl[i], lvl[i]);
//    zweidee::fbuf2d.setpixel(zweidee::data, 16+x, 12+y, color[lvl[i]].r, color[lvl[i]].g, color[lvl[i]].b);
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
