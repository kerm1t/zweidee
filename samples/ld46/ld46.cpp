#include "stdafx.h"

#define FBUF2D_WIDTH  64 // HACK!! remove!!
#define FBUF2D_HEIGHT 48 // HACK!! remove!!
#include "zweidee_draw.h"
#include "ld46.h"

int ld46::CLD46::init()
{
  return TRUE;
}

// ---------------
// main game cycle
// ---------------
int ld46::CLD46::doit(unsigned char * data)
{
///  draw_bg(data);
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
  bool hitwall = false;
  for (int i = 0; i < lvl_size; i++) if (lvl[(hero.box.y - 1)*lvl_w + hero.box.x] != 0) hitwall = true;
  if ((hero.box.y > 0) && (!hitwall)) hero.box.y--;
  hero.dir = 1;
  return true;
}

int ld46::CLD46::down()
{
  if (hero.state == hero::explode) return true;
  if (hero.box.y + hero.box.w < fbuf2d->height-1) hero.box.y++;
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
