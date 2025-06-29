//////////////////////////////
// main.cpp
// - random noise
//////////////////////////////

#include "stdafx.h"

// buffer dimension - define before include zweidee.h!
#define FBUF2D_WIDTH  64
#define FBUF2D_HEIGHT 48

#include "zweidee.h"    // draw to 2D buffer
#include "engine.h"     //   run a 2D game

/////////////////////
// put your code here
/////////////////////

zweidee::CEngine m_engine;
void do_stuff_here() // b) function
{
  // no rapid fire :-)
  if (GetAsyncKeyState(VK_UP))    m_engine.up();
  if (GetAsyncKeyState(VK_DOWN))  m_engine.down();
  if (GetAsyncKeyState(VK_LEFT))  m_engine.left();
  if (GetAsyncKeyState(VK_RIGHT)) m_engine.right();

  m_engine.move(); // move (i.e. increase playengine step) and update fbuf2d
}

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
//  zweidee::app_init(FBUF2D_WIDTH * 10, FBUF2D_HEIGHT * 10, hInstance, hPrevInstance, lpCmdLine, nCmdShow, WndProc);

  m_engine.init(&zweidee::fbuf2d, zweidee::data);

  ////////////////
  // run in thread 
  ////////////////

//  zweidee::repeat = randomnoise;
  zweidee::repeat = do_stuff_here;


  return zweidee::app_run(hInstance);
}
