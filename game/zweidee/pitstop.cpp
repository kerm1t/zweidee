#include "stdafx.h"

#include "pitstop.h"

unsigned char _y = 0;

int pitstop::CPitstop::init()
{
  return true;
}

int pitstop::CPitstop::doit(unsigned char * data)
{
  unsigned char r, g, b;
  for (unsigned int y = 0; y < fbuf2d.height; y++)
  {
    g = 255;
    if ((_y+y) % 4 < 2)
    {
      r = 0;
      b = 0;
    }
    else
    {
      r = 155;
      b = 155;
    }
    for (unsigned int x = 0; x < fbuf2d.width; x++)
    {
      fbuf2d.setpixel(data, x, y, r, g, b);
      fbuf2d.setpixel(data, x, y, r, g, b);
    }
  }
  _y--;
  if (_y == 0) _y = fbuf2d.height;
  return true;
}

int pitstop::CPitstop::left()
{
  return true;
}

int pitstop::CPitstop::right()
{
  return true;
}

int pitstop::CPitstop::fire()
{
  return true;
}
