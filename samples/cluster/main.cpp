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
  zweidee::app_init(FBUF2D_WIDTH * 10, FBUF2D_HEIGHT * 10, hInstance, hPrevInstance, lpCmdLine, nCmdShow);


  ///////////////
  // Init
  ///////////////

  fillData();

  /////////////////
  // hand over code
  /////////////////

  zweidee::doit = do_stuff_here; // your function -> function pointer
  _beginthread(zweidee::RenderThread, 0, 0);


  return zweidee::app_run(hInstance);
}
