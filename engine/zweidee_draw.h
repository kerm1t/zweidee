#pragma once

#include "zweidee.h"

namespace zweidee
{
  #define BRES_ARR_MAX 1000

  void Bresenham(FrameBuf2D * fbuf, int x0, int y0, int x1, int y1,
    const glm::vec3 col, unsigned char * data) // https://de.wikipedia.org/wiki/Bresenham-Algorithmus
	{
		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy, e2; // error value e_xy

		while (true)
		{
			fbuf->setpixel(data, x0, y0, col.r, col.g, col.b);

			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 > dy) { err += dy; x0 += sx; } // e_xy+e_x > 0
			if (e2 < dx) { err += dx; y0 += sy; } // e_xy+e_y < 0
		}
	}

	void Bresenham_Circle(FrameBuf2D * fbuf, int x0, int y0, int radius,
    const glm::vec3 col, uint8 * data) // https://de.wikipedia.org/wiki/Bresenham-Algorithmus
	{
		int f = 1 - radius;
		int ddF_x = 0;
		int ddF_y = -2 * radius;
		int x = 0;
		int y = radius;

		fbuf->setpixel(data, x0, y0 + radius, col.r, col.g, col.b);
		fbuf->setpixel(data, x0, y0 - radius, col.r, col.g, col.b);
		fbuf->setpixel(data, x0 + radius, y0, col.r, col.g, col.b);
		fbuf->setpixel(data, x0 - radius, y0, col.r, col.g, col.b);

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

			fbuf->setpixel(data, x0 + x, y0 + y, col.r, col.g, col.b); // oct. 6
			fbuf->setpixel(data, x0 - x, y0 + y, col.r, col.g, col.b); // 2
			fbuf->setpixel(data, x0 + x, y0 - y, col.r, col.g, col.b); // 1
			fbuf->setpixel(data, x0 - x, y0 - y, col.r, col.g, col.b); // 5
			fbuf->setpixel(data, x0 + y, y0 + x, col.r, col.g, col.b); // 7
			fbuf->setpixel(data, x0 - y, y0 + x, col.r, col.g, col.b); // 3
			fbuf->setpixel(data, x0 + y, y0 - x, col.r, col.g, col.b); // 4
			fbuf->setpixel(data, x0 - y, y0 - x, col.r, col.g, col.b); // 8
		}
	}

  // this one should be private
  // it draws a ...
  void Bresenham_arr(FrameBuf2D * fbuf, int x0, int y0, const int x1, const int y1, int * xarr,
    const glm::vec3 col, uint8 * data) // https://de.wikipedia.org/wiki/Bresenham-Algorithmus
  {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; // error value e_xy

    while (true)
    {
      fbuf->setpixel(data,x0,y0,col.r,col.g,col.b);
      if ((y0 > 0) && (y0 < BRES_ARR_MAX))
        xarr[y0] = x0; // <-- wichtig! x-pos merken!

      if (x0 == x1 && y0 == y1) break;
      e2 = 2 * err;
      if (e2 > dy) { err += dy; x0 += sx; } // e_xy+e_x > 0
      if (e2 < dx) { err += dx; y0 += sy; } // e_xy+e_y < 0
    }
  }

  struct vec2
  {
    vec2() : x(0.0f), y(0.0f) {}
    vec2(float _x, float _y) : x(_x), y(_y) {}
    struct { float x; float y; };

  };
  vec2 operator-(const vec2 v1, const vec2 v2)
  {
    return vec2(v2.x - v1.x, v2.y - v1.y);
  }

  void Triangle_filled(FrameBuf2D * fbuf, const vec2 p0, const vec2 p1, const vec2 p2,
    const glm::vec3 col, uint8 * data)
  {
    // sortiere nach y
    vec2 top, middle, bottom, tmp;
    if (p1.y < p0.y)
    {
      top = p1;
      middle = p0;
    }
    else
    {
      top = p0;
      middle = p1;
    }
    if (p2.y < middle.y)
    {
      if (p2.y < top.y)
      {
        bottom = middle;
        middle = top;
        top = p2;
      }
      else
      {
        bottom = middle;
        middle = p2;
      }
    }
    else
    {
      bottom = p2;
    }

    int xarr[BRES_ARR_MAX]; // 2do: vector instead of fixed array (depends on screen dimensions)
    int xarr2[BRES_ARR_MAX];
    Bresenham_arr(fbuf, top.x, top.y, middle.x, middle.y, xarr, col, data);
    Bresenham_arr(fbuf, top.x, top.y, bottom.x, bottom.y, xarr2, col, data);
    Bresenham_arr(fbuf, middle.x, middle.y, bottom.x, bottom.y, xarr, col, data);
    for (int y = top.y; y < bottom.y; y++)
    {
      if ( // quite a rough test
        (y > 0) &&
        (y < BRES_ARR_MAX) //&&
//        (xarr[y] > -100) &&
//        (xarr2[y] > -100) &&
        // rough pos test
//        (xarr[y] < 100) &&
//        (xarr2[y] < 100)
        )
      Bresenham(fbuf, xarr[y], y, xarr2[y], y, col, data); // man kann sogar noch einfacher mit x-Schleife schreiben
    }
  }

}