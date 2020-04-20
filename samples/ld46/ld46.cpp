#include "stdafx.h"

#define FBUF2D_WIDTH  64 // HACK!! remove!!
#define FBUF2D_HEIGHT 48 // HACK!! remove!!
#include "zweidee_draw.h"
#include "ld46.h"

// idea: autogenerate a level like in "worlds hardest game"
zweidee::point ld46::CLD46::prep_lvl3(zweidee::point seed)
{
  int w = 5 + rand() % (ld46::lvl_w - 20);
  int h = 5 + rand() % (ld46::lvl_h - 20);
  rect_ r = {
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
  case 0: return{ r.l, yhalf };
  case 1: return{ r.r, yhalf };
  case 2: return{ r.t, xhalf };
  case 3: return{ r.b, xhalf };
  }
  return{ 0, 0 };
}

int ld46::CLD46::init()
{
  // level
  lvl = new char[lvl_size];
  for (int i = 0; i < lvl_size; i++)
  {
    lvl[i] = 50 + rand() % 30; // 0..80
  }
  zweidee::point pt = { lvl_w / 2, lvl_h / 2 };
  for (int i = 0; i < 11; i++)
  {
    pt = prep_lvl3(pt);
  }

  return TRUE;
}

// ---------------
// main game cycle
// ---------------
int ld46::CLD46::doit(unsigned char * data)
{
  draw_bg(data);
  draw_hero(data);
  return TRUE;
}

int ld46::CLD46::left()
{
  if (hero.state == hero::explode) return true;
  if (hero.box.x > 0) hero.box.x--;
  hero.dir = 1;
  return true;
}

int ld46::CLD46::right()
{
  if (hero.state == hero::explode) return true;
  if (hero.box.x + hero.box.w < fbuf2d->width-1) hero.box.x++;
  hero.dir = 2;
  return true;
}

int ld46::CLD46::up()
{
  if (hero.state == hero::explode) return true;
//  bool hitwall = false;
//  for (int i = 0; i < lvl_size; i++) if (lvl[(hero.box.y - 1)*lvl_w + hero.box.x] != 0) hitwall = true; // <-- stupid!!
  bool hitwall = lvl[(hero.box.y - 1)*lvl_w + hero.box.x] < 100;
  if ((hero.box.y > 0) && (!hitwall)) hero.box.y--;
  hero.dir = 1;
  return true;
}

int ld46::CLD46::down()
{
  if (hero.state == hero::explode) return true;
  bool hitwall = false;// lvl[(hero.box.y + 1)*lvl_w + hero.box.x] < 100;
  if ((hero.box.y + hero.box.w < fbuf2d->height-1) && (!hitwall)) hero.box.y++;
  hero.dir = 2;
  return true;
}
int ld46::CLD46::fire()
{
  return true;
}

int ld46::CLD46::draw_hero(unsigned char * data)
{
  int _x = hero.box.x;
  int _y = hero.box.y;
  glm::vec3 col = {255,155,155};
  fbuf2d->setpixel(data, _x, _y, col.r, col.g, col.b); // bgr
  return true;
}

int ld46::CLD46::draw_bg(unsigned char * data)
{
  for (int i = 0; i < lvl_size; i++)
  {
    int x = (FBUF2D_WIDTH - lvl_w) / 2 + i % lvl_w;
    int y = (FBUF2D_HEIGHT - lvl_h) / 2 + i / lvl_w;
    char col = lvl[i];
    fbuf2d->setpixel(data, x, y, col, col, col);
  }
  return true;
}
