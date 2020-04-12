//////////////////////////////
// main.cpp
// - fill data structure
// - find cluster with k-means
//////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "zweidee.h"    // draw to 2D buffer

#include <windows.h>    // Header File For Windows

#define GLEW_STATIC
#include <GL/glew.h>

#include <process.h>    // _beginthread

// forward declaration of functions in this code module
ATOM             MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

zweidee::CRender m_render;

// buffer dimension
#define FBUF2D_WIDTH 128
#define FBUF2D_HEIGHT 32
#define FBUF2D_PIXELS FBUF2D_WIDTH * FBUF2D_HEIGHT
#define FBUF2D_SIZE   FBUF2D_PIXELS * 3 // r,g,b
// windows dimension (the later may be changed with resize)
int win_w = FBUF2D_WIDTH * 10;
int win_h = FBUF2D_HEIGHT * 10;
bool b_WM_resized = false;


//////////////////////////
// put your variables here
//////////////////////////
#define FPA_SIZE  FBUF2D_PIXELS
#define FPA_WIDTH FBUF2D_WIDTH
int FPA[FPA_SIZE];

zweidee::colrgb colcluster[3] = { {255,0,0},{0,225,0},{0,0,233} };


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

float eudistance(zweidee::point p1, zweidee::point p2)
{
  return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
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

  m_render.Setup_Geometry(texID);



  ///////////////
  // Init
  ///////////////

  // background (dark)
  srand(12);
  for (int i = 0; i < FPA_SIZE; i++)
  {
    FPA[i] = rand()/600;
  }

  // obj.1
  FPA[5 * FPA_WIDTH + 10] = 255;
  FPA[5 * FPA_WIDTH + 11] = 255;
  FPA[5 * FPA_WIDTH + 12] = 255;
  FPA[6 * FPA_WIDTH + 10] = 255;
  FPA[6 * FPA_WIDTH + 11] = 255;
  FPA[6 * FPA_WIDTH + 12] = 255;
  FPA[7 * FPA_WIDTH + 12] = 255;
  FPA[8 * FPA_WIDTH + 12] = 200;
  FPA[9 * FPA_WIDTH + 12] = 220;
  FPA[10 * FPA_WIDTH + 12] = 220;

  // obj.2
  FPA[24 * FPA_WIDTH + 100] = 255;
  FPA[24 * FPA_WIDTH + 101] = 255;
  FPA[25 * FPA_WIDTH + 100] = 200;
  FPA[25 * FPA_WIDTH + 101] = 200;
  FPA[25 * FPA_WIDTH + 102] = 255;
  FPA[26 * FPA_WIDTH + 100] = 255;
  FPA[26 * FPA_WIDTH + 101] = 255;
  FPA[26 * FPA_WIDTH + 102] = 255;

  // obj.3
  FPA[14 * FPA_WIDTH + 90] = 155;
  FPA[14 * FPA_WIDTH + 91] = 155;
  FPA[14 * FPA_WIDTH + 92] = 255;
  FPA[15 * FPA_WIDTH + 90] = 255;
  FPA[15 * FPA_WIDTH + 91] = 255;
  FPA[15 * FPA_WIDTH + 92] = 155;
  FPA[16 * FPA_WIDTH + 90] = 155;
  FPA[16 * FPA_WIDTH + 91] = 255;
  FPA[16 * FPA_WIDTH + 92] = 255;

  // write FPA to buf2d
  for (int i = 0; i < FPA_SIZE; i++)
  {
    int x = i % FPA_WIDTH;
    int y = i / FPA_WIDTH;
    zweidee::fbuf2d.setpixel(zweidee::data, x, y, FPA[i], FPA[i], FPA[i]);
  }
  // map to texture --> shift to zweidee.h
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);   // hier gibt es Schwierigkeiten mit .bmp,
  ///////////////
  // Init
  ///////////////


// (1) https://medium.com/datadriveninvestor/how-the-simplest-clustering-algorithm-work-with-code-b8af21aabda2
// (2) http://www.goldsborough.me/c++/python/cuda/2017/09/10/20-32-46-exploring_k-means_in_python,_c++_and_cuda/
#define MAXCLUSTER 3
  zweidee::point kmean[MAXCLUSTER];// = { {10,10},{100,30},{64,16} };
  zweidee::point input[100];
  int numinput = 0;
  // a) get relevant points
  for (int i = 0; i < FPA_SIZE; i++)
  {
    if (FPA[i] > 150)
    {
      unsigned int x = i % FPA_WIDTH;
      unsigned int y = i / FPA_WIDTH;
      input[numinput] = { x, y };
      numinput++;
    }
  }
  // b) init k-means
  for (int j = 0; j < MAXCLUSTER; j++)
  {
    int n = rand() % numinput;
    kmean[j] = input[n];
  }
  // c) init
  float mindist[FPA_SIZE];
  int belongtocluster[FPA_SIZE];
  for (int i = 0; i < FPA_SIZE; i++)
    mindist[i] = 255.0f; // set to a value that is > max. distance      
  memset(belongtocluster, 255, FPA_SIZE); // set to a value that is > #cluster
  // c) loop 1
  for (int i = 0; i < numinput; i++)
  {
    for (int j = 0; j < MAXCLUSTER; j++)
    {
        float dst = eudistance(input[i], kmean[j]);
        if (dst < mindist[i])
        {
          mindist[i] = dst;
          belongtocluster[i] = j;
        }
    }
  }

  for (int i = 0; i < numinput; i++)
  {
    zweidee::point pt = input[i];
    zweidee::colrgb col = colcluster[belongtocluster[i]];
    zweidee::fbuf2d.setpixel(zweidee::data, pt.x, pt.y, col.r, col.g, col.b);
  }
  // map to texture --> shift to zweidee.h
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);   // hier gibt es Schwierigkeiten mit .bmp,





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
      break;
    case 37: // ARROW-LEFT
      break;
    case 39: // ARROW-RIGHT
      break;
    case 79: // O >> Step
      break;
    case 80: // P >> Pause ON/OFF
      break;
    case 87: // W
      break;
    case 65: // A
      break;
    case 83: // S
      break;
    case 68: // D
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
