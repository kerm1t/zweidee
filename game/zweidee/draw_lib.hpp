#pragma once

#include "math.h"
#include "2dframebuf.hpp"

int draw_progress(unsigned char * data)
{
  /*  char r, g, b;
  r = g = b = 255;
  for (int i = 0; i < iloopy; i++)
  {
  int x = 10+i;
  fbuf2D.setPixel(data, x, 64/2-1, r, g, b);
  fbuf2D.setPixel(data, x, 64 / 2, r, g, b);
  fbuf2D.setPixel(data, x, 64 / 2 + 1, r, g, b);
  }
  */  return true;
}

int draw_arrow(unsigned char * data)
{
  /*  char r, g, b;

  for (int i = 0; i < 64; i++)
  {
  r = ((i + 1) % 3) * 255;
  g = (i % 3) * 255;
  b = ((i - 1) % 3) * 255;
  fbuf2D.setPixel(data,                i, (iloopy + i) % 64, r, g, b);  //   "\"   left part of arrow
  fbuf2D.setPixel(data, (2 * 64 - 2) - i, (iloopy + i) % 64, r, g, b);  //   "/"  right part of arrow
  }
  */  return true;
}

void Bresenham(FrameBuf2D * fbuf, int x0, int y0, int x1, int y1, const glm::vec3 col, unsigned char * data) // https://de.wikipedia.org/wiki/Bresenham-Algorithmus
{
    int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2; /* error value e_xy */

//    float32 f_intensity = 255.0f; // <-- 16bit?
    while (true)
    {
        fbuf->setPixel(data,x0,y0,255,255,255);
//      Set_Cell_on_Freelayer(x0,y0,(T_UINT8)f_intensity);

        if (x0==x1 && y0==y1) break;
        e2 = 2*err;
        if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}

void Bresenham_Circle(FrameBuf2D * fbuf, int x0, int y0, int radius, const glm::vec3 col, unsigned char * data) // https://de.wikipedia.org/wiki/Bresenham-Algorithmus
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    fbuf->setPixel(data,x0, y0 + radius,col.r,col.g,col.b);
    fbuf->setPixel(data,x0, y0 - radius,col.r,col.g,col.b);
    fbuf->setPixel(data,x0 + radius, y0,col.r,col.g,col.b);
    fbuf->setPixel(data,x0 - radius, y0,col.r,col.g,col.b);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        fbuf->setPixel(data,x0 + x, y0 + y,col.r,col.g,col.b); // oct. 6
        fbuf->setPixel(data,x0 - x, y0 + y,col.r,col.g,col.b); // 2
        fbuf->setPixel(data,x0 + x, y0 - y,col.r,col.g,col.b); // 1
        fbuf->setPixel(data,x0 - x, y0 - y,col.r,col.g,col.b); // 5
        fbuf->setPixel(data,x0 + y, y0 + x,col.r,col.g,col.b); // 7
        fbuf->setPixel(data,x0 - y, y0 + x,col.r,col.g,col.b); // 3
        fbuf->setPixel(data,x0 + y, y0 - x,col.r,col.g,col.b); // 4
        fbuf->setPixel(data,x0 - y, y0 - x,col.r,col.g,col.b); // 8
    }
}
