//////////////////////////////
// main.cpp
// - random noise
//////////////////////////////

#include "stdafx.h"

// buffer dimension - define before include zweidee.h!
#define FBUF2D_WIDTH  64
#define FBUF2D_HEIGHT 48

#include "zweidee.h"
#include "engine.h"
#include "ld46.h"
#include <time.h> // rand()

/////////////////////
// put your code here
/////////////////////

zweidee::CEngine m_engine;

void do_stuff_here() // b) function
{
  // no (rapid) fire :-)
  if (GetAsyncKeyState(VK_UP))    m_engine.up();
  if (GetAsyncKeyState(VK_DOWN))  m_engine.down();
  if (GetAsyncKeyState(VK_LEFT))  m_engine.left();
  if (GetAsyncKeyState(VK_RIGHT)) m_engine.right();

  m_engine.move(); // move (i.e. increase playengine step) and update fbuf2d
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  zweidee::app_init(640, 480, hInstance, hPrevInstance, lpCmdLine, nCmdShow);

  // initialize rand() once at progstart to reproduce, use e.g. srand(0) to get to a defined chain;
  srand((unsigned int)time(NULL));

  m_engine.init(&zweidee::fbuf2d, zweidee::data);

  // background / frame
  for (int i = 0; i < FBUF2D_PIXELS; i++)
  {
    int x = i % FBUF2D_WIDTH;
    int y = i / FBUF2D_WIDTH;
    zweidee::fbuf2d.setpixel(zweidee::data, x, y, 111, 111, 111);
  }
  
  ////////////////
  // do not :-) run in thread 
  ////////////////

  zweidee::repeat = do_stuff_here;

//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);   // hier gibt es Schwierigkeiten mit .bmp,
//  zweidee::m_render.Render();

  int res = zweidee::app_run(hInstance);
  delete ld46::lvl;
  return res;
}
