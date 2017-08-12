#pragma once

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
