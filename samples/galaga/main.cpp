////////////////////////////
// main.cpp
// - independent render loop
//   - engine.move
//   - engine.render
////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "zweidee.h"    // draw to 2D buffer
#include "engine.h"	    //   run a 2D game

#include <windows.h>    // Header File For Windows
//#include <windowsx.h>   // GET_X_LPARAM, GET_Y_LPARAM

#define GLEW_STATIC
#include <GL/glew.h>

#include <process.h>    // _beginthread


zweidee::Engine m_engine;


int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow)
{
  zweidee::call_up = m_engine.up;
  zweidee::call_down = m_engine.down;
  zweidee::call_left = m_engine.left;
  zweidee::call_right = m_engine.right;

  zweidee::call_doit = m_engine.move;

//  return (int) msg.wParam;
}


