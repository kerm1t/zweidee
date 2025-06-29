//////////////////////////////
// main.cpp
// - random noise
//////////////////////////////

#include "stdafx.h"

// buffer dimension - define before include zweidee.h!
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
//int main(int argc, char** argv) 
{
  if (!zweidee::app_init(640, 480, hInstance, hPrevInstance, lpCmdLine, nCmdShow))
  {
    std::cout << "init failed" << std::endl;
    return false;
  }

  ////////////////
  // run in thread 
  ////////////////

  zweidee::repeat = randomnoise;


  return zweidee::app_run(hInstance);
}
