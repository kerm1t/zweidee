#pragma once

#include "zweidee.h"

namespace zweidee
{
	void Bresenham(FrameBuf2D * fbuf, int x0, int y0, int x1, int y1,
    const glm::vec3 col, unsigned char * data) // https://de.wikipedia.org/wiki/Bresenham-Algorithmus
	{
		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy, e2; // error value e_xy

		while (true)
		{
			fbuf->setpixel(data, x0, y0, 255, 255, 255);

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
}