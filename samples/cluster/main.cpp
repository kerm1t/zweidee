////////////////////////////////
// main.cpp
// - fill data structure
// - find cluster with k-means
// - estimate number of clusters
////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "zweidee.h"    // draw to 2D buffer

#include <windows.h>    // Header File For Windows

#define GLEW_STATIC
#include <GL/glew.h>

#include <process.h>    // _beginthread
#include <time.h>       // srand()

#include "cluster.h"

// forward declaration of functions in this code module
ATOM             MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

zweidee::CRender m_render;

//////////////////////////
// put your variables here
//////////////////////////



dword lasttickcount = 0;
dword accumulatedTimeSinceLastUpdate = 0;

// Fix Timing
// https://gafferongames.com/post/fix_your_timestep/

// OpenGL calls moved to own thread
// s. http://stackoverflow.com/questions/9833852/opengl-game-loop-multithreading
void RenderThread(void *args)
{
  lasttickcount = GetTickCount();
  while (true)
  {
    accumulatedTimeSinceLastUpdate += (GetTickCount() - lasttickcount); // GetTickCount [ms] dword
    lasttickcount = GetTickCount();

    if (accumulatedTimeSinceLastUpdate > 12) // indep. from gfx-card -> update every 12 [ms]
    {
      accumulatedTimeSinceLastUpdate = 0;

      ////////////////
      // do stuff here
      ////////////////
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

      ////////////////
      // do stuff here
      ////////////////
    }

    if (b_WM_resized)
    {
      m_render.ReSizeGLScene(win_w, win_h);
      b_WM_resized = false;
    }

    m_render.Render(); // render update-rate independent from move() (s. above)
  }
  _endthread();
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  MSG msg;
  HACCEL hAccelTable;

  zweidee::hRC = NULL;
  zweidee::hDC = NULL;
  zweidee::hWnd = NULL;
  zweidee::fps = 0.0f;
  zweidee::lastTime = 0.0f;

  // init global strings
  LoadString(hInstance, IDS_APP_TITLE, zweidee::szTitle, MAX_LOADSTRING);  // window title etc. see .rc
  LoadString(hInstance, IDC_ZWEIDEE, zweidee::szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  if (!zweidee::InitInstance(hInstance, nCmdShow, win_w, win_h)) // init application
  {
    return FALSE;
  }

  m_render.Init(win_w, win_h);

  zweidee::data = new unsigned char[FBUF2D_SIZE]; // size = pixels*3 (r,g,b)
  GLuint texID = zweidee::fbuf2d.Init(FBUF2D_WIDTH, FBUF2D_HEIGHT);
  memset(zweidee::data, 0, FBUF2D_SIZE);

  m_render.Setup_Geometry(texID);



  ///////////////
  // Init
  ///////////////

  fillData();
  
  ///////////////
  // Init
  ///////////////






  // stuff is done here
  _beginthread(RenderThread, 0, 0);




  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZWEIDEE));
  // main message loop
  while (GetMessage(&msg, NULL, 0, 0))
  {
    wglMakeCurrent(NULL, NULL); // <-- no other access to OpenGL here!! --> only in RenderThread 

    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // --> rendering moved to RenderThread, otherwise no autonomous object-movement possible
  }

  delete zweidee::data;

  return (int)msg.wParam;
}

// autogenerated: register window class (mandatory)
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZWEIDEE));
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCE(IDC_ZWEIDEE);
  wcex.lpszClassName = zweidee::szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassEx(&wcex);
}

// moved to zweidee.h --> BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, int w, int h)

// autogenerated: process main window's messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;

  switch (message)
  {
  case WM_MOUSEWHEEL:
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645617(v=vs.85).aspx
    break;
  case WM_MOUSEMOVE:
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645616(v=vs.85).aspx
    break;
  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    // process menu choice
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
    //	case WM_PAINT:
    // ... painting by OpenGL
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
    win_w = LOWORD(lParam);
    win_h = HIWORD(lParam);
    // resize --> in the thread now, as from here no access to OpenGL-context
    b_WM_resized = true;
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// moved to zweidee.h --> INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
