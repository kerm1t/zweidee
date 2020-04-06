// zweidee.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "stdafx.h"

#include "zweidee.h"
#include "proj.h"

#include <windows.h>                            // Header File For Windows
#include <windowsx.h>                           // GET_X_LPARAM, GET_Y_LPARAM

#define GLEW_STATIC
#include <GL/glew.h>

#include <process.h> // _beginthread

#define MAX_LOADSTRING 100

// Globale Variablen:
HINSTANCE        hInst;                         // Aktuelle Instanz
TCHAR            szTitle[MAX_LOADSTRING];       // Titelleistentext
TCHAR            szWindowClass[MAX_LOADSTRING]; // Klassenname des Hauptfensters

// Vorwärtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM             MyRegisterClass(HINSTANCE hInstance);
BOOL             InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

HGLRC            hRC=NULL;                      // Permanent Rendering Context
HDC              hDC=NULL;                      // Private GDI Device Context
HWND             hWnd=NULL;                     // Holds Our Window Handle

proj::Proj m_proj;

//Camera m_cam;

int win_h,win_w;
int mouse_x,mouse_y;

int iT = 1;

static float framesPerSecond = 0.0f;            // This will store our fps
static float lastTime = 0.0f;                   // This will hold the time from the last frame
dword lasttickcount = 0;
dword accumulatedTimeSinceLastUpdate = 0;

bool b_WM_resized = false;

void CalculateFrameRate()
{
  float currentTime = GetTickCount() * 0.001f;    
  ++framesPerSecond;
  if( currentTime - lastTime > 1.0f )
  {
    lastTime = currentTime;
    framesPerSecond = 0.0;
  }
}

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

//      if (GetAsyncKeyState(VK_SPACE)) m_proj.fire(); // Dauerfeuer!
      if (GetAsyncKeyState(VK_UP))    m_proj.up();
      if (GetAsyncKeyState(VK_DOWN))  m_proj.down();
      if (GetAsyncKeyState(VK_LEFT))  m_proj.left();
      if (GetAsyncKeyState(VK_RIGHT)) m_proj.right();

      m_proj.move();
    }

    if (b_WM_resized)
    {
      m_proj.m_render.ReSizeGLScene(win_w,win_h);
      b_WM_resized = false;
    }
//    m_cam.update_View(); // View = Pos,At,Norm

    m_proj.render(); // render update-rate independent from move() (s. above) 
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

  // TODO: Hier Code einfügen.
  MSG msg;
  HACCEL hAccelTable;

  // Globale Zeichenfolgen initialisieren
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);    // Fenstertitel etc. siehe .rc
  LoadString(hInstance, IDC_GLSHOOT, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Anwendungsinitialisierung ausführen:
  if (!InitInstance (hInstance, nCmdShow))
  {
    return FALSE;
  }

  RECT rect; 
  if (GetClientRect(hWnd, &rect))
  { 
    win_w = rect.right - rect.left; 
    win_h = rect.bottom - rect.top; 
  }
  mouse_x = (int)(win_w/2.0f); // init
  mouse_y = (int)(win_h/2.0f);

  m_proj.m_render.width = win_w;
  m_proj.m_render.height = win_h;
  hDC = m_proj.m_render.GL_attach_to_DC(hWnd); // <== NeHe    

  glewExperimental = GL_TRUE; // <-- Nutzen?
  glewInit(); // <-- takes a little time

  m_proj.init();	// <-- Textures erst nach glewInit() laden!!
                  // a) data loading + b) data description c) render.Init()
  
  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GLSHOOT));

  _beginthread(RenderThread, 0, 0);

  // Hauptnachrichtenschleife:
  while (GetMessage(&msg, NULL, 0, 0))
  {
    wglMakeCurrent(NULL,NULL); // <-- no other access to OpenGL here!! --> only in RenderThread 

    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // --> Projection moved to RenderThread, otherwise no autonomous object-movement possible
  }

  return (int) msg.wParam;
}

//
//  FUNKTION: MyRegisterClass()
//
//  ZWECK: Registriert die Fensterklasse.
//
//  KOMMENTARE:
//
//    Sie müssen die Funktion verwenden, wenn Sie möchten, dass der Code
//    mit Win32-Systemen kompatibel ist, bevor die RegisterClassEx-Funktion
//    zu Windows 95 hinzugefügt wurde. Der Aufruf der Funktion ist wichtig,
//    damit die kleinen Symbole, die mit der Anwendung verknüpft sind,
//    richtig formatiert werden.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style			= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= WndProc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= hInstance;
  wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GLSHOOT));
  wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GLSHOOT);
  wcex.lpszClassName	= szWindowClass;
  wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassEx(&wcex);
}

//
//   FUNKTION: InitInstance(HINSTANCE, int)
//
//   ZWECK: Speichert das Instanzenhandle und erstellt das Hauptfenster.
//
//   KOMMENTARE:
//
//        In dieser Funktion wird das Instanzenhandle in einer globalen Variablen gespeichert, und das
//        Hauptprogrammfenster wird erstellt und angezeigt.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  //   HWND hWnd;

  hInst = hInstance; // Instanzenhandle in der globalen Variablen speichern

#define galaga
#ifdef frogger
  int _w = 600; // frogger: 600, other: 800
  int _h = 800;
#endif
#ifdef galaga
  int _w = 512;
  int _h = 512;
#endif
  // center on screen
  RECT rect;
  GetClientRect(GetDesktopWindow(), &rect);
  int _x = (rect.right / 2) - (_w / 2);
  int _y = (rect.bottom / 2) - (_h / 2);
  // center on screen
  hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, _x, _y, _w, _h, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

//
//  FUNKTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ZWECK:  Verarbeitet Meldungen vom Hauptfenster.
//
//  WM_COMMAND	- Verarbeiten des Anwendungsmenüs
//  WM_PAINT	- Zeichnen des Hauptfensters
//  WM_DESTROY	- Beenden-Meldung anzeigen und zurückgeben
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  int zDelta;
  int pt_x,pt_y;

  glm::vec3 vVehDirNorm,vVehDirOrth;

  switch (message)
  {
  case WM_MOUSEWHEEL: // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645617(v=vs.85).aspx
    zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
//    if (zDelta > 0) m_cam.Pos.z += 0.5; else m_cam.Pos.z -= 0.5; 
    break;
  case WM_MOUSEMOVE:
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645616(v=vs.85).aspx
    pt_x = GET_X_LPARAM(lParam); // LOWORD u. HIWORD fkt. nicht bei mehreren Monitoren!
    pt_y = GET_Y_LPARAM(lParam);
    mouse_x = pt_x;
    mouse_y = pt_y;
    break;
  case WM_COMMAND:
    wmId    = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    // Menüauswahl bearbeiten:
    switch (wmId)
    {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
      m_proj.fire(); // Einzelfeuer
      break;
    case 37: // ARROW-LEFT
      break;
    case 39: // ARROW-RIGHT
      break;
    case 79: // O >> Step
      m_proj.bStep = true;
      break;
    case 80: // P >> Pause ON/OFF
      m_proj.bPause = !(m_proj.bPause);
      break;
    case 87: // W
      break;
    case 65: // A
//      m_cam.TurnLeft();
//      m_cam.StrafeLeft();
      break;
    case 83: // S
//      m_cam.MoveBack();
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

// Meldungshandler für Infofeld.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
