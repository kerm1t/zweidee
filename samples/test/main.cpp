//////////////////////////////
// main.cpp
// - random noise
//////////////////////////////

#include "stdafx.h"
//#include "resource.h"

// buffer dimension - need to define before include zweidee.h, 2do: fix!!
#define FBUF2D_WIDTH  64
#define FBUF2D_HEIGHT 48

#include "zweidee.h"

/////////////////////
// put your code here
/////////////////////

void randomnoise()
{
  for (int i = 0; i < FBUF2D_PIXELS; i++)
  {
    int x = i % FBUF2D_WIDTH;
    int y = i / FBUF2D_WIDTH;
    zweidee::fbuf2d.setpixel(zweidee::data, x, y, rand(), rand(), rand());
  }
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  zweidee::app_init(FBUF2D_WIDTH * 10, FBUF2D_HEIGHT * 10, hInstance, hPrevInstance, lpCmdLine, nCmdShow);


  /////////////////
  // hand over code
  /////////////////

  zweidee::doit = randomnoise; // your function -> function pointer


  return zweidee::app_run(hInstance);
}
