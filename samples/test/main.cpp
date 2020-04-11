////////////////////////////
// main.cpp
// - independent render loop
//   - engine.move
//   - engine.render
////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "zweidee.h"    // draw to 2D buffer
//#include "engine.h"     //   run a 2D game

#include <windows.h>    // Header File For Windows
//#include <windowsx.h>   // GET_X_LPARAM, GET_Y_LPARAM

#define GLEW_STATIC
#include <GL/glew.h>

#include <process.h>    // _beginthread

// forward declaration of functions in this code module
ATOM             MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//zweidee::Engine m_engine;
zweidee::CRender m_render;


// Galaga screen dimension, 2do --> hand over to zweide::engine/ ::render!!
#define FBUF2D_WIDTH 64
#define FBUF2D_HEIGHT 48
#define FBUF2D_SIZE FBUF2D_WIDTH * FBUF2D_HEIGHT
// windows dimension (the later may be changed with resize
int win_w = FBUF2D_WIDTH * 10;
int win_h = FBUF2D_HEIGHT * 10;
bool b_WM_resized = false;


///////////////
// Init
///////////////



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

      for (int i = 0; i < FBUF2D_SIZE; i++)
      {
        int x = i % FBUF2D_WIDTH;
        int y = i / FBUF2D_WIDTH;
        zweidee::fbuf2d.setpixel(zweidee::data, x, y, rand(), rand(), rand());
      }
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);


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


  ///////////////
  // Init
  ///////////////

  // from engine contructor!!
//  zweidee::fbuf2d.width = FBUF2D_WIDTH; // 2do: this shall be from the input
//  zweidee::fbuf2d.height = FBUF2D_HEIGHT;
//  zweidee::fbuf2d.imagesize = zweidee::fbuf2d.width * zweidee::fbuf2d.height * 3;
//  fbuf2d = &m_game.fbuf2d;                     // fbuf part of game (e.g. galaga)
//  zweidee::data = new unsigned char[zweidee::fbuf2d.imagesize]; // data part of proj <-- 2do




  // 2do: this should be zweidee -->
  m_render.width = win_w; // this will size the viewport
  m_render.height = win_h;
  zweidee::hDC = m_render.GL_attach_to_DC(zweidee::hWnd); // <== NeHe    

  glewExperimental = GL_TRUE; // <-- Nutzen?
  glewInit(); // <-- takes a little time

              // 2do: this should be zweidee -->
//  m_engine.init();	// <-- Textures erst nach glewInit() laden!!
                    // a) data loading + b) data description c) render.Init()
  m_render.Init(); // InitGL + Initshaders, kann auch spaeter aufgerufen werden...
  m_render.FPS(); // <-- wenn ich das ins VAO fuelle, gibt's nen Fehler (erst mit dem neuen ShaderFPS)
                  //     beim LoadObjects(s.u.) call
  GLuint texID = zweidee::fbuf2d.Init(FBUF2D_WIDTH, FBUF2D_HEIGHT);

  zweidee::data = new unsigned char[zweidee::fbuf2d.imagesize]; // data part of proj <-- 2do

  m_render.vGLTexture.push_back(texID);
  m_render.Bind_VBOs_to_VAOs(); // now hand over VBO's to VAO's
  ///////////////
  // Init
  ///////////////





  ///////////////
  // Do stuff
  ///////////////
  _beginthread(RenderThread, 0, 0);
  ///////////////
  // Do stuff
  ///////////////




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
