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
#include <time.h>       // srand()

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
#define FPA_SIZE   FBUF2D_PIXELS
#define FPA_HEIGHT FBUF2D_HEIGHT
#define FPA_WIDTH  FBUF2D_WIDTH
int FPA[FPA_SIZE];

zweidee::colrgb colcluster[6] = { {255,0,0},{0,225,0},{0,0,233},{255,255,0},{0,150,255},{255,0,225} };


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
  srand(time(NULL));

  // background (dark)
  for (int i = 0; i < FPA_SIZE; i++)
  {
    FPA[i] = rand() % 100;// 255;
  }

  // objects: set seeds and dilate them
  int numgenclusters = rand() % 10;
  for (int c = 0; c < numgenclusters; c++)
  {
    int x = rand() % FPA_WIDTH-1;
    int y = rand() % FPA_HEIGHT-1;
    FPA[y * FPA_WIDTH + x] = 255;
    for (int d = 0; d < 25; d++)
    {
      int x1 = x + rand() % 4;
      int y1 = y + rand() % 4;
      FPA[y1 * FPA_WIDTH + x1] = 255;
    }
  }

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
#define MAXCLUSTER 10 // (pretend) we don't know the number of clusters, which is makes it much more difficult!
  // prepare: init datastructures
  int numinput = 0;
  zweidee::point input[FPA_SIZE];
  zweidee::point kmean[MAXCLUSTER];// = { {10,10},{100,30},{64,16} };
  float mindist[FPA_SIZE];
  int belongtocluster[FPA_SIZE];
  for (int i = 0; i < FPA_SIZE; i++) mindist[i] = 255.0f; // set to a value that is > max. distance      
  memset(belongtocluster, 255, FPA_SIZE); // set to a value that is > #cluster

  // prepare: get relevant points
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

  // a) init k-means
  for (int j = 0; j < MAXCLUSTER; j++)
  {
    int n = rand() % numinput;
    kmean[j] = input[n];
  }
  // b) measure eucl. distance for each datapt. to each k-mean
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
  // c) update k-mean (optional)
#ifdef UPDATE_KMEAN
  for (int j = 0; j < MAXCLUSTER; j++)
  {
    zweidee::point tmp = {0,0};
    int numclusterpoints = 0;
    for (int i = 0; i < numinput; i++)
    {
      if (j == belongtocluster[i])
      {
        tmp.x += input[i].x;
        tmp.y += input[i].y;
        numclusterpoints++;
      }
      kmean[j].x = tmp.x / (GLfloat)numclusterpoints;
      kmean[j].y = tmp.y / (GLfloat)numclusterpoints;
    }
  }
#endif

  // d) merge clusters (i.e. dist < 2)
  zweidee::point clustermerge[MAXCLUSTER];
  unsigned int numclustermerge = 0;
  for (unsigned int j = 0; j < MAXCLUSTER; j++)
  {
    // run through all cluster points
    zweidee::point pt1 = { 0,0 };
    for (unsigned int p = 0; p < numinput; p++)
    {
      if (j == belongtocluster[p])
      {
        pt1.x = input[p].x;
        pt1.y = input[p].y;

        // run through all other cluster's points
        for (unsigned int k = 0; k < MAXCLUSTER; k++)
        {
          if (k != j)
          {
            // do not run again through cluster 2-1, when 1-2 was done already
            for (unsigned int q = 0; q < numinput; q++)
            {
              if (k == belongtocluster[q])
              {
                if ((input[q].x == pt1.x+1) &&
                    (input[q].y == pt1.y))
                {
                  // check, if this pair already in clustermerge-array
                  bool pairfound = false;
                  for (unsigned int l = 0; l < MAXCLUSTER; l++)
                    if (((clustermerge[l].x == j) && (clustermerge[l].y == k)) ||
                      ((clustermerge[l].x == k) && (clustermerge[l].y == j))) pairfound = true;
                  if (!pairfound) clustermerge[numclustermerge++] = { j,k }; // merge j and k
                  // 2do: update clusters right here!
                  // and tag, that these clusters were merged
                }
                if ((input[q].x == pt1.x-1) &&
                  (input[q].y == pt1.y))
                {
                  // check, if this pair already in clustermerge-array
                  bool pairfound = false;
                  for (unsigned int l = 0; l < MAXCLUSTER; l++)
                    if (((clustermerge[l].x == j) && (clustermerge[l].y == k)) ||
                      ((clustermerge[l].x == k) && (clustermerge[l].y == j))) pairfound = true;
                  if (!pairfound) clustermerge[numclustermerge++] = { j,k }; // merge j and k
                  // 2do: update clusters right here!
                }
                if ((input[q].x == pt1.x) &&
                  (input[q].y == pt1.y+1))
                {
                  // check, if this pair already in clustermerge-array
                  bool pairfound = false;
                  for (unsigned int l = 0; l < MAXCLUSTER; l++)
                    if (((clustermerge[l].x == j) && (clustermerge[l].y == k)) ||
                      ((clustermerge[l].x == k) && (clustermerge[l].y == j))) pairfound = true;
                  if (!pairfound) clustermerge[numclustermerge++] = { j,k }; // merge j and k
                  // 2do: update clusters right here!
                }
                if ((input[q].x == pt1.x) &&
                  (input[q].y == pt1.y-1))
                {
                  // check, if this pair already in clustermerge-array
                  bool pairfound = false;
                  for (unsigned int l = 0; l < MAXCLUSTER; l++)
                    if (((clustermerge[l].x == j) && (clustermerge[l].y == k)) ||
                      ((clustermerge[l].x == k) && (clustermerge[l].y == j))) pairfound = true;
                  if (!pairfound) clustermerge[numclustermerge++] = { j,k }; // merge j and k
                  // 2do: update clusters right here!
                }
              }
            }
          }
        } // INNER LOOP: for (unsigned int k = 0; k < MAXCLUSTER; k++)
      }
    }
  } // OUTER LOOP: for (unsigned int j = 0; j < MAXCLUSTER; j++)

  // update clusters
  for (unsigned int j = 0; j < MAXCLUSTER; j++)
  {
    for (unsigned int p = 0; p < numinput; p++)
    {
      if (belongtocluster[p] == clustermerge[j].y) belongtocluster[p] = clustermerge[j].x;
    }
  }

  int numclusters = MAXCLUSTER - numclustermerge; // YEP, done it!
  char buf[20];
  sprintf(buf, "%d clusters found", numclusters);
  int msgboxID = MessageBox(
    NULL,
    buf,
    "k-means + dilation",
    MB_ICONINFORMATION | MB_OK
  );


// Fundamental problem: we do not know the number of clusters in advance!
  // idea: make a connected component analysis, i.e.

  // wrap up: i) color the pixel according to found clusters
  for (int i = 0; i < numinput; i++)
  {
    zweidee::point pt = input[i];
    zweidee::colrgb col = colcluster[belongtocluster[i]];
    zweidee::fbuf2d.setpixel(zweidee::data, pt.x, pt.y, col.r, col.g, col.b);
  }
  // ii) color cluster centers in white
//#define DRAW_CLUSTERCENTER
#ifdef DRAW_CLUSTERCENTER
  for (int j = 0; j < MAXCLUSTER; j++)
  {
    zweidee::fbuf2d.setpixel(zweidee::data, kmean[j].x, kmean[j].y, 255, 255, 255);
    std::cout << j << kmean[j].x << kmean[j].y << std::endl;
  }
#endif
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
