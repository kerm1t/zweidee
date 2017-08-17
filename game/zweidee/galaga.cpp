#include "stdafx.h"

#include "galaga.h"

//#include "math.h"

int galaga::CGalaga::init()
{
  a_episode_len[0] = fbuf2d.width - (2 * 10); // size of progress bar ;-)
  a_episode_active[0] = false;
  iepisode = 0;
  iloopy = 0;

  spacecraft.num = 4;
  spacecraft.dir = 0;
  spacecraft.box.x = (int)(fbuf2d.width/2) - (int)7/2;
  spacecraft.box.y = 60-1;
  spacecraft.box.w = 7;
  spacecraft.box.h = 3;

  idx_shots = 0;
  // enemies:
  //   xxx
  //    x
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    a_enemies[i].on = true;
    a_enemies[i].box.x = 3 + i * 8;
    a_enemies[i].box.y = i;
    a_enemies[i].box.w = 3;
    a_enemies[i].box.h = 2;
  }

  return TRUE;
}

int galaga::CGalaga::doit(unsigned char * data)
{
  /*
      draw some stuff [...]
  */

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
    for (unsigned int i = 0; i<spacecraft.num; i++) fbuf2d.setPixel(data, 10 + i * 2, 5, 255, 255, 255); // bgr
    switch (spacecraft.dir)
    {
    case 0: draw_spacecraft(data); break;
    case 1: draw_spacecraft_turnleft(data); break;
    case 2: draw_spacecraft_turnright(data); break;
    }
    spacecraft.dir = 0; // set to neutral again
    move_shots();
    collisioncheck();  // 1st Collisioncheck, after Shot-move
    draw_shots(data);
    move_enemies();
    draw_enemies(data);
    collisioncheck();  // 2nd Collisioncheck after enemies move (if test only once, shot and enemy might just bypass each other in the same cycle ;-)
    if (spacecraft.num == 0) iepisode++;
    break;

  case 2:
    break;
  }

  iloopy++;

  return TRUE;
}

int galaga::CGalaga::left()
{
  if (spacecraft.box.x > 0) spacecraft.box.x--;
  spacecraft.dir = 1;
  return true;
}

int galaga::CGalaga::right()
{
  if (spacecraft.box.x + spacecraft.box.w < fbuf2d.width) spacecraft.box.x++;
  spacecraft.dir = 2;
  return true;
}

int galaga::CGalaga::fire()
{
  a_shots[idx_shots].on = true;
  a_shots[idx_shots].box.x = spacecraft.box.x + (int)(spacecraft.box.w/2);
  a_shots[idx_shots].box.y = spacecraft.box.y;
  idx_shots++;
  if (idx_shots == NUM_SHOTS) idx_shots = 0; // wrap around (ringbuffer)
  return true;
}

int galaga::CGalaga::draw_starfield_vert(unsigned char * data) // cheap trick ... 2do: replace with random starfield
{
  char r, g, b;

  for (unsigned int i = 0; i < fbuf2d.height; i++)
  {
    r = ((i + 1) % 3) * 255;
    g = ( i      % 3) * 255;
    b = ((i - 1) % 3) * 255;
    fbuf2d.setPixel(data, cos((float)i) *                      i, (iloopy + i) % 64, r, g, b);
    fbuf2d.setPixel(data, cos((float)i) * (fbuf2d.width - 2) - i, (iloopy + i) % 64, r, g, b);
    // funny effect:
//    fbuf2d.setPixel(data, (unsigned int)cos(i) *                      i, (iloopy + i) % 64, r, g, b);
//    fbuf2d.setPixel(data, (unsigned int)cos(i) * (fbuf2D.width - 2) - i, (iloopy + i) % 64, r, g, b);
  }
  return true;
}

int galaga::CGalaga::draw_obj(const rect box, const unsigned char * obj, unsigned char * data)
{
  const unsigned char _w = box.w;
  const unsigned char _h = box.h;
  for (unsigned char y=0; y<_h; y++)
  {
    for (unsigned char x=0; x<_w; x++)
    {
      int _x = box.x + x; // upper ...
      int _y = box.y + y; // ... left
      unsigned char iA = y*_w+x;
      if (obj[iA] > 0)
      {
        glm::vec3 col = cols[obj[iA]];
        fbuf2d.setPixel(data, _x, _y, col.r, col.g, col.b); // bgr
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
  rect r = {spacecraft.box.x,spacecraft.box.y,5,3};
  draw_obj(r,aSpacecraft_turnleft,data);
  return true;
}

int galaga::CGalaga::draw_spacecraft_turnright(unsigned char * data)
{
  rect r = {spacecraft.box.x+2,spacecraft.box.y,5,3};
  draw_obj(r,aSpacecraft_turnright,data);
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
      fbuf2d.setPixel(data, x, y, r, g, b);
    }
  }
  return true;
}

int galaga::CGalaga::draw_enemies(unsigned char * data)
{
  for (unsigned int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].on)
    {
      rect r = {a_enemies[i].box.x,a_enemies[i].box.y,3,2};
      draw_obj(r,aEnemy,data);
    }
  }
  return true;
}

int galaga::CGalaga::move_enemies()
{
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].on)
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
    if (a_enemies[i].on)
    {
      for (int j = 0; j < NUM_SHOTS; j++)
      {
        if (a_shots[j].on)
        {
          if ((a_enemies[i].box.x == a_shots[j].box.x) &&
              (a_enemies[i].box.y == a_shots[j].box.y))
          {
            a_enemies[i].on = false;
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
      a_enemies[i].on = true;
      // comment to get "random" enemy position after 1st wave
      a_enemies[i].box.x = 3 + i * 8;
      a_enemies[i].box.y = i;
      // comment to get "random" enemy position after 1st wave
    }
  }

  // B) enemies vs. ship
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].on)
    {
      if ((a_enemies[i].box.x == spacecraft.box.x) &&
          (a_enemies[i].box.y == 60))
      {
        // explode!!
        spacecraft.num--;
        a_enemies[i].on = false;
      }
    }
  }

  return true;
}
