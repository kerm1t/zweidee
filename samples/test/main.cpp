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

void do_stuff_here() // b) function
{
  for (int i = 0; i < FBUF2D_PIXELS; i++)
  {
    int x = i % FBUF2D_WIDTH;
    int y = i / FBUF2D_WIDTH;
    zweidee::fbuf2d.setpixel(zweidee::data, x, y, rand(), rand(), rand());
  }
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);
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

  zweidee::doit = do_stuff_here; // your function -> function pointer
  _beginthread(zweidee::RenderThread, 0, 0);



  return zweidee::app_run(hInstance);
}
