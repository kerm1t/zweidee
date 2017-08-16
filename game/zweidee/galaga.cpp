#include "stdafx.h"

#include "galaga.h"

//#include "math.h"

/*galaga::CGalaga(FrameBuf2D & _fbuf2D)
  : CGame(_fbuf2D)
{
};*/

int galaga::CGalaga::init()
{
  a_episode_len[0] = fbuf2d.width - (2 * 10); // size of progress bar ;-)
  a_episode_active[0] = false;
  iepisode = 0;
  iloopy = 0;

  SpacecraftNum = 4;
  xSpaceCraft = (int)(fbuf2d.width / 2);
  SpacecraftDir = 0;
  idx_shots = 0;
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    a_enemies[i].on = true;
    a_enemies[i].x = 3 + i * 8;
    a_enemies[i].y = i;
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
    for (unsigned int i = 0; i<SpacecraftNum; i++) fbuf2d.setPixel(data, 10 + i * 2, 5, 255, 255, 255); // bgr
    switch (SpacecraftDir)
    {
    case 0: draw_spacecraft(data); break;
    case 1: draw_spacecraft_turnleft(data); break;
    case 2: draw_spacecraft_turnright(data); break;
    }
    SpacecraftDir = 0; // set to neutral again
    move_shots();
    collisioncheck();  // 1st Collisioncheck, after Shot-move
    draw_shots(data);
    move_enemies();
    draw_enemies(data);
    collisioncheck();  // 2nd Collisioncheck after enemies move (if test only once, shot and enemy might just bypass each other in the same cycle ;-)
    if (SpacecraftNum == 0) iepisode++;
    break;

  case 2:
    break;
  }

  iloopy++;

  return TRUE;
}

int galaga::CGalaga::left()
{
  if (xSpaceCraft > 3) xSpaceCraft--;
  SpacecraftDir = 1;
  return true;
}

int galaga::CGalaga::right()
{
  if (xSpaceCraft < fbuf2d.width - 3 - 1) xSpaceCraft++;
  SpacecraftDir = 2;
  return true;
}

int galaga::CGalaga::fire()
{
  a_shots[idx_shots].on = true;
  a_shots[idx_shots].x = xSpaceCraft;
  a_shots[idx_shots].y = 60 - 2;
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

//
//     xxx
//     xxx
//
//     x   xx   x
//     xxxxxxxxxx
int galaga::CGalaga::draw_spacecraft(unsigned char * data)
{
  unsigned char r, g, b;

  const unsigned char _w = 7;
  const unsigned char _h = 3;
  const unsigned char a[_w*_h] = {
    0,0,1,1,1,0,0,
    2,0,1,3,1,0,2,
    2,4,1,1,1,4,2
  };
  for (unsigned char y=0; y<_h; y++)
  {
    for (unsigned char x=0; x<_w; x++)
    {
      int _x = xSpaceCraft-int(_w/2) + x;
      int _y = 60 - int(_h/2) + y;
      unsigned char iA = y*_w+x;
  // Anton's proposal
      if (a[iA] == 0) ;
      if (a[iA] == 1) { r = g = 155; b = 255; } // hellblau
      if (a[iA] == 2) { r = g = 110; b = 255; } // mittelblau
      if (a[iA] == 3) { r = 255; g = b = 0;   } // rot
      if (a[iA] == 4) { r = g = 80;  b = 255; } // dunkelblau
      if (a[iA] > 0)
      {
        fbuf2d.setPixel(data, _x, _y, r, g, b); // bgr
      }
    }
  }

  return true;
}

int galaga::CGalaga::draw_spacecraft_turnleft(unsigned char * data)
{
  unsigned char r, g, b;
  b = 255; g = r = 155;

  int x = xSpaceCraft;
  int y = 60;

  // Anton's proposal
  fbuf2d.setPixel(data, x - 1, y - 1, r, g, b); // bgr
  fbuf2d.setPixel(data, x, y - 1, r, g, b); // bgr
  fbuf2d.setPixel(data, x + 1, y - 1, r, g, b); // bgr

  fbuf2d.setPixel(data, x - 2, y, 110, 110, 255);
  fbuf2d.setPixel(data, x - 1, y, r, g, b);
  fbuf2d.setPixel(data, x, y, r, 0, 0);
  fbuf2d.setPixel(data, x + 1, y, r, g, b);
  fbuf2d.setPixel(data, x + 2, y, 190, 190, 255);

  fbuf2d.setPixel(data, x - 2, y + 1, 110, 110, 255);
  fbuf2d.setPixel(data, x - 1, y + 1, r, g, b);
  fbuf2d.setPixel(data, x, y + 1, r, g, b);
  fbuf2d.setPixel(data, x + 1, y + 1, r, g, b);
  fbuf2d.setPixel(data, x + 2, y + 1, 190, 190, 255);

  return true;
}

int galaga::CGalaga::draw_spacecraft_turnright(unsigned char * data)
{
  unsigned char r, g, b;
  b = 255; g = r = 155;

  int x = xSpaceCraft;
  int y = 60;

  // Anton's proposal
  fbuf2d.setPixel(data, x - 1, y - 1, r, g, b); // bgr
  fbuf2d.setPixel(data, x, y - 1, r, g, b); // bgr
  fbuf2d.setPixel(data, x + 1, y - 1, r, g, b); // bgr

  fbuf2d.setPixel(data, x - 2, y, 190, 190, 255);
  fbuf2d.setPixel(data, x - 1, y, r, g, b);
  fbuf2d.setPixel(data, x, y, r, 0, 0);
  fbuf2d.setPixel(data, x + 1, y, r, g, b);
  fbuf2d.setPixel(data, x + 2, y, 110, 110, 255);

  fbuf2d.setPixel(data, x - 2, y + 1, 190, 190, 255);
  fbuf2d.setPixel(data, x - 1, y + 1, r, g, b);
  fbuf2d.setPixel(data, x, y + 1, r, g, b);
  fbuf2d.setPixel(data, x + 1, y + 1, r, g, b);
  fbuf2d.setPixel(data, x + 2, y + 1, 110, 110, 255);

  return true;
}

int galaga::CGalaga::move_shots()
{
  for (unsigned int i = 0; i < NUM_SHOTS; i++)
  {
    if (a_shots[i].on)
    {
      a_shots[i].y--;
      if (a_shots[i].y == 0) a_shots[i].on = false;
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
      int x = a_shots[i].x;
      int y = a_shots[i].y;
      fbuf2d.setPixel(data, x, y, r, g, b);
    }
  }
  return true;
}

int galaga::CGalaga::draw_enemies(unsigned char * data)
{
  unsigned char r, g, b;
  r = 255; g = b = 0;
  for (unsigned int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].on)
    {
      int x = a_enemies[i].x;
      int y = a_enemies[i].y;
      fbuf2d.setPixel(data, x - 1, y - 1, r, g, b);
      fbuf2d.setPixel(data, x,     y - 1, r, g, b);
      fbuf2d.setPixel(data, x + 1, y - 1, r, g, b);
      fbuf2d.setPixel(data, x,     y,     r, g, b);
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
      if ((iloopy % 2) == 1)        a_enemies[i].y++;
      if (a_enemies[i].y == 64 - 2) a_enemies[i].y = 0;
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
          if ((a_enemies[i].x == a_shots[j].x) &&
              (a_enemies[i].y == a_shots[j].y))
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
      a_enemies[i].x = 3 + i * 8;
      a_enemies[i].y = i;
      // comment to get "random" enemy position after 1st wave
    }
  }

  // B) enemies vs. ship
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    if (a_enemies[i].on)
    {
      if ((a_enemies[i].x == xSpaceCraft) &&
        (a_enemies[i].y == 60))
      {
        // explode!!
        SpacecraftNum--;
        a_enemies[i].on = false;
      }
    }
  }

  return true;
}
