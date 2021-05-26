////////////////////////////
// main.cpp (Glenz)
// - independent render loop
//   - engine.move
//   - engine.render
////////////////////////////

#include "stdafx.h"
#include "resource.h"

// buffer dimension
#define FBUF2D_WIDTH  64
#define FBUF2D_HEIGHT 64

#include "zweidee.h"    // draw to 2D buffer
#include "engine.h"     //   run a 2D game

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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

int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  zweidee::app_init(FBUF2D_WIDTH * 10, FBUF2D_HEIGHT * 10, hInstance, hPrevInstance, lpCmdLine, nCmdShow, WndProc);
  
  m_engine.init(&zweidee::fbuf2d, zweidee::data);


  /////////////////
  // hand over code
  /////////////////
  zweidee::repeat = do_stuff_here; // your function -> function pointer

  return zweidee::app_run(hInstance);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;

  switch (message)
  {
  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    switch (wmId)
    {
    case IDM_ABOUT:
      DialogBox(zweidee::hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, zweidee::About);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    // case WM_PAINT: ... painting by OpenGL
  case WM_KEYDOWN:
    switch (wParam)
    {
    case 32: // Space
      m_engine.fire(); // single fire
      break;
    case 79: // O >> Step
      m_engine.bStep = true;
      break;
    case 80: // P >> Pause ON/OFF
      m_engine.bPause = !(m_engine.bPause);
      break;
    }
    break;
  case WM_SIZE:
    zweidee::win_w = LOWORD(lParam);
    zweidee::win_h = HIWORD(lParam);
    // resize --> in the thread now, as from here no access to OpenGL-context
    zweidee::b_WM_resized = true;
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
