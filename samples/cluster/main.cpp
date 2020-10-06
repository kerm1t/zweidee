////////////////////////////////
// main.cpp
// - fill data structure
// - find cluster with k-means
// - estimate number of clusters
////////////////////////////////

#include "stdafx.h"
//#include "resource.h"

// buffer dimension - need to define before include zweidee.h, 2do: fix!!
#define FBUF2D_WIDTH 128
#define FBUF2D_HEIGHT 64

#include "zweidee.h"    // draw to 2D buffer
#include "cluster.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////////////
// put your code here
/////////////////////

void do_stuff_here() // b) function
{
  if (mode == dmCluster)
  {
    // method a)
    //        kmeans_merge();
    // method b)
    gridSweep();
    mode = dmDone;
  }

  if (mode == dmRefill)
  {
    fillData();
    mode = dmInit;
  }
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  zweidee::app_init(FBUF2D_WIDTH * 10, FBUF2D_HEIGHT * 10, hInstance, hPrevInstance, lpCmdLine, nCmdShow, WndProc);


  ///////////////
  // Init
  ///////////////

  fillData();

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
      if      (mode == dmInit) mode = dmCluster;
      else if (mode == dmDone) mode = dmRefill;
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