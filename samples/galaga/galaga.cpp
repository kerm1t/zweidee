#include "stdafx.h"

#define FBUF2D_WIDTH  64 // HACK!! remove!!
#define FBUF2D_HEIGHT 64 // HACK!! remove!!
#include "zweidee_draw.h"
#include "galaga.h"

int galaga::CGalaga::init()
{
  a_episode_len[0] = fbuf2d->width - (2 * 10); // size of progress bar ;-)
  a_episode_active[0] = false;
  iepisode = 0;
  iloopy = 0;

  spacecraft.state = spacecraft::normal;
  spacecraft.lives = 4;
  spacecraft.dir = 0;
  spacecraft.box.x = (int)(fbuf2d->width/2) - (int)7/2;
  spacecraft.box.y = 60-1;
  spacecraft.box.w = 7;
  spacecraft.box.h = 3;

  idx_shots = 0;
  // enemies:
  //   xxx
  //    x
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    a_enemies[i].state = enemy::on;
    a_enemies[i].box.x = 3 + i * 8;
    a_enemies[i].box.y = i;
    a_enemies[i].box.w = 3;
    a_enemies[i].box.h = 2;
  }

  for (int i = 0; i < NUM_SHOTS; i++)
  {
    a_shots[i].box.w = 1;
    a_shots[i].box.h = 1;
  }

  return TRUE;
 }                    

// ---------------
// main game cycle
// ---------------
int galaga::CGalaga::doit(unsigned char * data)
{
  switch (iepisode)
  {
  case 0:
    if (!a_episode_active[0])
    {
      iloopy = a_episode_len[iepisode];
    }
    else
    {
//    lib.draw_progress(data);
    }
    if (iloopy >= a_episode_len[iepisode]) iepisode++;
    break;

  case 1:
//     lib.draw_arrow(data);
    draw_starfield_vert(data);
    for (unsigned int i = 0; i<spacecraft.lives; i++) fbuf2d->setpixel(data, 10 + i * 2, 5, 255, 255, 255); // bgr
    if (spacecraft.state == spacecraft::normal)
    {
      switch (spacecraft.dir)
      {
      case 0: draw_spacecraft(data); break;
      case 1: draw_spacecraft_turnleft(data); break;
      case 2: draw_spacecraft_turnright(data); break;
      }
    }
    else if (spacecraft.state == spacecraft::explode)
    {
      draw_spacecraft_explode(data);
      spacecraft.explode_counter--;
      if (spacecraft.explode_counter == 0)
      {
        spacecraft.state = spacecraft::normal;
        spacecraft.lives--; // reduce only after explosion fully executed ;-)
      }
    }
    else ;
    spacecraft.dir = 0; // set to neutral again
    move_shots();
//    collisioncheck();  // 1st Collisioncheck, after Shot-move
    draw_shots(data);
    move_enemies();
    draw_enemies(data);
    collisioncheck();  // 2nd Collisioncheck after enemies move (if test only once, shot and enemy might just bypass each other in the same cycle ;-)
    if (spacecraft.lives == 0) iepisode++;
    break;

  case 2:
    draw_gameover(data);
    break;
  }

  iloopy++;

  return TRUE;
}

int galaga::CGalaga::left()
{
  if (spacecraft.state == spacecraft::explode) return true;
  if (spacecraft.box.x > 0) spacecraft.box.x--;
  spacecraft.dir = 1;
  return true;
}

int galaga::CGalaga::right()
{
  if (spacecraft.state == spacecraft::explode) return true;
  if (spacecraft.box.x + spacecraft.box.w < fbuf2d->width) spacecraft.box.x++;
  spacecraft.dir = 2;
  return true;
}

int galaga::CGalaga::up()
{
  return true;
}
int galaga::CGalaga::down()
{
  return true;
}

int galaga::CGalaga::fire()
{
  if (iepisode == 2)
  {
    init(); // restart!
  }
  else
  {
    if (spacecraft.state == spacecraft::normal)
    {
      a_shots[idx_shots].on = true;
      a_shots[idx_shots].box.x = spacecraft.box.x + (int)(spacecraft.box.w/2);
      a_shots[idx_shots].box.y = spacecraft.box.y;
      idx_shots++;
      if (idx_shots == NUM_SHOTS) idx_shots = 0; // wrap around (ringbuffer)
    }
  }
  return true;
}

int galaga::CGalaga::draw_starfield_vert(unsigned char * data) // cheap trick ... 2do: replace with random starfield
{
  uint8 r, g, b;

  for (uint32 i = 0; i < fbuf2d->height; i++)
  {
    r = ((i + 1) % 3) * 255;
    g = ( i      % 3) * 255;
    b = ((i - 1) % 3) * 255;
    fbuf2d->setpixel(data, cos((float)i) *                       i, (iloopy + i) % 64, r, g, b);
    fbuf2d->setpixel(data, cos((float)i) * (fbuf2d->width - 2) - i, (iloopy + i) % 64, r, g, b);
  }
  return true;
}

int galaga::CGalaga::draw_obj(const game::rect box, const unsigned char * obj, unsigned char * data)
{
  const uint8 _w = box.w;
  const uint8 _h = box.h;
  for (uint8 y=0; y<_h; y++)
  {
    for (uint8 x=0; x<_w; x++)
    {
      int _x = box.x + x; // upper ...
      int _y = box.y + y; // ... left
      unsigned char iA = y*_w+x;
      if (obj[iA] > 0)
      {
        glm::vec3 col = cols[obj[iA]];
        fbuf2d->setpixel(data, _x, _y, col.r, col.g, col.b); // bgr
      }
    }
  }
  return true;
}

//        xxx   
//     x  xox  x
//     xxxxxxxxx
int galaga::CGalaga::draw_spacecraft(unsigned char * data)
{
  draw_obj(spacecraft.box,aSpacecraft,data);
  return true;
}

int galaga::CGalaga::draw_spacecraft_turnleft(unsigned char * data)
{
  game::rect r = {spacecraft.box.x,spacecraft.box.y,5,3};
  draw_obj(r,aSpacecraft_turnleft,data);
  return true;
}

int galaga::CGalaga::draw_spacecraft_turnright(unsigned char * data)
{
  game::rect r = {spacecraft.box.x+2,spacecraft.box.y,5,3};
  draw_obj(r,aSpacecraft_turnright,data);
  return true;
}

int galaga::CGalaga::draw_spacecraft_explode(unsigned char * data)
{
  int x = spacecraft.box.x + (int)(spacecraft.box.w/2);
  int y = spacecraft.box.y + (int)(spacecraft.box.h/2);
  glm::vec3 col = glm::vec3(255,255,0);
  Bresenham_Circle(fbuf2d,x-5+(rand()%10),y-5+(rand()%10),rand()%10,col,data); // several circles with differtent pos, radius
  return true;
}

int galaga::CGalaga::draw_gameover(unsigned char * data)
{
  game::rect r = {10,10,17,9};
  draw_obj(r,aGameover,data);
  return true;
}

int galaga::CGalaga::move_shots()
{
  for (unsigned int i = 0; i < NUM_SHOTS; i++)
  {
    if (a_shots[i].on)
    {
      a_shots[i].box.y--;
      if (a_shots[i].box.y == 0) a_shots[i].on = false;
    }
  }
  return true;
}

int galaga::CGalaga::draw_shots(unsigned char * data)
{
  unsigned char r, g, b;
  g = 255; r = b = 0;
  for (unsigned int i = 0; i < NUM_SHOTS; i++)
  {
    if (a_shots[i].on)
    {
      int x = a_shots[i].box.x;
      int y = a_shots[i].box.y;
      fbuf2d->setpixel(data, x, y, r, g, b);
    }
  }
  return true;
}

int galaga::CGalaga::draw_enemy_explosion(const unsigned int i, unsigned char * data)
{
  int x = a_enemies[i].box.x + (int)(a_enemies[i].box.w/2);
  int y = a_enemies[i].box.y + (int)(a_enemies[i].box.h/2);
  glm::vec3 col = glm::vec3(255,100,0);
  Bresenham_Circle(fbuf2d,x,y,rand()%10,col,data);
  a_enemies[i].explode_counter--;
  if (a_enemies[i].explode_counter == 0) a_enemies[i].state = enemy::off;
  return true;
}

int galaga::CGalaga::draw_enemies(unsigned char * data)
{
  for (unsigned int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].state == enemy::on)
    {
      game::rect r = {a_enemies[i].box.x,a_enemies[i].box.y,3,2};
      draw_obj(r,aEnemy,data);
    }
    else if (a_enemies[i].state == enemy::explode)
    {
      draw_enemy_explosion(i,data);
    }
    else ;
  }
  return true;
}

int galaga::CGalaga::move_enemies()
{
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].state == enemy::on)
    {
      if ((iloopy % 2) == 1) a_enemies[i].box.y++;
      if (a_enemies[i].box.y == 64 - 2) a_enemies[i].box.y = 0;
    }
  }
  return true;
}

int galaga::CGalaga::collisioncheck()
{
  // A) shots vs. enemies
  int iOff = 0;
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].state == enemy::on)
    {
      for (int j = 0; j < NUM_SHOTS; j++)
      {
        if (a_shots[j].on)
        {
          if (DoBoxesIntersect(a_enemies[i].box,a_shots[j].box))
          {
            a_enemies[i].state = enemy::explode;
            a_enemies[i].explode_counter = 50;
            a_shots[j].on = false;
          }
        }
      }
    }
    else
    {
      iOff++;
    }
  }
  if (iOff == NUM_ENEMIES)
  {
    // all shot --> next wave
    for (int i = 0; i < NUM_ENEMIES; i++)
    {
      a_enemies[i].state = enemy::on;
      // comment to get "random" enemy position after 1st wave
      a_enemies[i].box.x = 3 + i * 8;
      a_enemies[i].box.y = i;
      // comment to get "random" enemy position after 1st wave
    }
  }

  // B) enemies vs. ship
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].state == enemy::on)
    {
      if (DoBoxesIntersect(a_enemies[i].box,spacecraft.box))
      {
        spacecraft.state = spacecraft::explode;
        spacecraft.explode_counter = 100;
        a_enemies[i].state = enemy::explode;
        a_enemies[i].explode_counter = 50;
      }
    }
  }

  return true;
}
