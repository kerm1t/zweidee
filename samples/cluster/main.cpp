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

// forward declaration of functions in this code module
ATOM             MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

zweidee::CRender m_render;

// buffer dimension, this has double the height to show
// a) input,                 rows  0..31
// b) cluster search result, rows 21..63
#define FBUF2D_WIDTH 128
#define FBUF2D_HEIGHT 64
#define FBUF2D_PIXELS FBUF2D_WIDTH * FBUF2D_HEIGHT
#define FBUF2D_SIZE   FBUF2D_PIXELS * 3 // r,g,b
// windows dimension (the later may be changed with resize)
int win_w = FBUF2D_WIDTH * 10;
int win_h = FBUF2D_HEIGHT * 10;
bool b_WM_resized = false;


//////////////////////////
// put your variables here
//////////////////////////
#define FPA_HEIGHT 32
#define FPA_WIDTH  FBUF2D_WIDTH
#define FPA_SIZE   FPA_WIDTH*FPA_HEIGHT
int FPA[FPA_SIZE];
#define MAXCLUSTER 10 // (pretend) we don't know the number of clusters, which is makes it much more difficult!
#define CLUSTER_GUI_ROW1 32

zweidee::colrgb colcluster[10] = { {255,0,0},{0,225,0},{0,0,255},{255,255,0},{0,255,255},
                                   {255,0,225},{125,0,0},{0,125,0},{0,0,125},{125,125,0} };

bool bCluster = false;
bool bRefill = false;


float eudistance(zweidee::point p1, zweidee::point p2)
{
  return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}

// (1) https://medium.com/datadriveninvestor/how-the-simplest-clustering-algorithm-work-with-code-b8af21aabda2
// (2) http://www.goldsborough.me/c++/python/cuda/2017/09/10/20-32-46-exploring_k-means_in_python,_c++_and_cuda/
void kmeans_merge()
{
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
  //  --> done after merging

  // Fundamental problem: we do not know the number of clusters in advance!
  // idea: make a connected component analysis, i.e.

  // d) merge clusters (i.e. dist < 2)
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
                if ((input[q].x == pt1.x + 1) &&
                  (input[q].y == pt1.y))
                {
                  // cluster-update
                  for (unsigned int h = 0; h < numinput; h++)
                  {
                    if (belongtocluster[h] == k) belongtocluster[h] = j;
                  }
                  break;
                }
                if ((input[q].x == pt1.x - 1) &&
                  (input[q].y == pt1.y))
                {
                  // cluster-update
                  for (unsigned int h = 0; h < numinput; h++)
                  {
                    if (belongtocluster[h] == k) belongtocluster[h] = j;
                  }
                  break;
                }
                if ((input[q].x == pt1.x) &&
                  (input[q].y == pt1.y + 1))
                {
                  // cluster-update
                  for (unsigned int h = 0; h < numinput; h++)
                  {
                    if (belongtocluster[h] == k) belongtocluster[h] = j;
                  }
                  break;
                }
                if ((input[q].x == pt1.x) &&
                  (input[q].y == pt1.y - 1))
                {
                  // cluster-update
                  for (unsigned int h = 0; h < numinput; h++)
                  {
                    if (belongtocluster[h] == k) belongtocluster[h] = j;
                  }
                  break;
                }
              }
            }
          }
        } // INNER LOOP: for (unsigned int k = 0; k < MAXCLUSTER; k++)
      }
    }
  } // OUTER LOOP: for (unsigned int j = 0; j < MAXCLUSTER; j++)


  // e) count clusters
  int numclusters = 0;
  int clusters[MAXCLUSTER];
  for (unsigned int j = 0; j < MAXCLUSTER; j++) clusters[j] = 255;
  for (unsigned int p = 0; p < numinput; p++)
  {
    if ((belongtocluster[p] == -1)) break;
    bool found = false;
    for (unsigned int j = 0; j < MAXCLUSTER; j++)
    {
      if (belongtocluster[p] == clusters[j])
      {
        found = true;
      }
    }
    if (!found) clusters[numclusters++] = belongtocluster[p];
  }


  char buf[55];
  sprintf(buf, "random k-mean initialization, k=%d:\r\n%d clusters found", MAXCLUSTER, numclusters);
  int msgboxID = MessageBox(
    NULL,
    buf,
    "k-means + dilation",
    MB_ICONINFORMATION | MB_OK
  );


  // f) update k-mean (optional)
#define UPDATE_KMEAN
#ifdef UPDATE_KMEAN
  for (int j = 0; j < MAXCLUSTER; j++)
  {
    zweidee::point tmp = { 0,0 };
    int numclusterpoints = 0;
    for (int i = 0; i < numinput; i++)
    {
      if (j == belongtocluster[i])
      {
        tmp.x += input[i].x;
        tmp.y += input[i].y;
        numclusterpoints++;
      }
    }
    kmean[j].x = tmp.x / (GLfloat)numclusterpoints;
    kmean[j].y = tmp.y / (GLfloat)numclusterpoints;
  }
#endif


  // wrap up: i) color the pixel according to found clusters
  for (int i = 0; i < numinput; i++)
  {
    zweidee::point pt = input[i];
    zweidee::colrgb col = colcluster[belongtocluster[i]];
    zweidee::fbuf2d.setpixel(zweidee::data, pt.x, CLUSTER_GUI_ROW1 + pt.y, col.r, col.g, col.b);
  }
  // ii) color cluster centers in white
#define DRAW_CLUSTERCENTER
#ifdef DRAW_CLUSTERCENTER
  for (int j = 0; j < MAXCLUSTER; j++)
  {
    zweidee::fbuf2d.setpixel(zweidee::data, kmean[j].x, CLUSTER_GUI_ROW1 + kmean[j].y, 255, 255, 255);
    std::cout << j << kmean[j].x << kmean[j].y << std::endl;
  }
#endif
  // map to texture --> shift to zweidee.h
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);   // hier gibt es Schwierigkeiten mit .bmp,
}

// --------- DB SCAN -----------

#define TMP_GRID_DIMENSION_U 128
#define TMP_GRID_DIMENSION_V 32
#define TMP_GRID_CELLS_PER_BUCKET 1 // :-)
#define TMP_MAX_NUM_CLUSTERS 10

typedef struct
{
  uint8  b_cells_square; // 1 if all cells are squares; 0 otherwise         
  uint32 ui_celldim;     // dimension of the coarse grid cell in the initial grid
  uint32 ui_ncell;       // number of relevant coarse grid cells
  uint16 a_cellcount[TMP_GRID_DIMENSION_U*TMP_GRID_DIMENSION_V / TMP_GRID_CELLS_PER_BUCKET]; // <-- 64 = 8x8 ?, depending on celldim ? 
} s_Grid_t;

typedef struct
{
  uint8           Pvisit   [TMP_GRID_DIMENSION_U * TMP_GRID_DIMENSION_V];
  uint16          neighbors[TMP_GRID_DIMENSION_U * TMP_GRID_DIMENSION_V + 3];
} s_DBSCAN_TMP_t;

typedef struct
{
  uint32 ui_id;
  uint32 ui_startIndex;
  uint32 ui_numPoints;
} s_Cluster_t;

//
//    clusterList               cellIDList (grdIndex)
//    ===========               ==========
//
//    clu    iCellList          iCell
//    ----+----                 ----------
//     1  |  0  -------------->  1381
//        |  2 (length)          1382
//     2  |  2  -------------->  1584
//        |  3 (length)          1585
//        |                      1586
//    ... |                      ...
//
typedef struct
{
  uint16 ui_numClusters;
  s_Cluster_t  as_clusters[TMP_MAX_NUM_CLUSTERS];  // max number of clusters
  uint32       a_grdIndex [TMP_GRID_DIMENSION_U * TMP_GRID_DIMENSION_V]; // can be adapted to something smaller <-- bullshit!
} s_ClusterList_t;

typedef struct
{
  // computation memory to large for stack
  s_Grid_t         grid;               // in
  s_DBSCAN_TMP_t   tmp_dbscan;
  s_ClusterList_t  coarse_clusterlist; // out
} s_DBGridSCAN_t;


// -------------------------------------------------------------------
// browse through Grid in waterfall like fashion,
// i.e. start from upper left and visit the right + three lower neighbours, i.e.
//
//     | X | N
//  ---+---+---
//   N | N | N
//
// plus special handling for left, right and lower boarder of Grid
// -------------------------------------------------------------------
void gridNeighbours(
  const s_Grid_t&   grid,
  const uint32      ui_minPointsCluster,
  s_DBSCAN_TMP_t&   tmp_dbscan,
  s_ClusterList_t&  s_clusterList)
{
  const uint32 npts = grid.ui_ncell;

  const uint32 nhcells = (uint32)ceil((float)TMP_GRID_DIMENSION_U / (float)grid.ui_celldim); // maybe necessary only for coarsegrid!?

  uint32 nClu = 0;      // 1st list: clusters
  uint32 nCells = 0;    // 2nd list: cells

                        // Reset Pvisit array.
  memset(tmp_dbscan.Pvisit, 0, sizeof(tmp_dbscan.Pvisit));

  for (uint32 n = 0; n < (npts - 1); ++n)
  {
    if (tmp_dbscan.Pvisit[n] == 0) // s_compMem has input grid-size (e.g. 1024 x 1024) ??
    {
      tmp_dbscan.Pvisit[n] = 1;  // mark as visited

      if (grid.a_cellcount[n] >= ui_minPointsCluster) // form a cluster..
      {
        s_clusterList.as_clusters[nClu].ui_id = nClu;
        s_clusterList.as_clusters[nClu].ui_startIndex = nCells;//n;
        s_clusterList.as_clusters[nClu].ui_numPoints = 1;

        s_clusterList.a_grdIndex[nCells] = n;
        nCells++;

        // Init Neighbours
        memset(tmp_dbscan.neighbors, 0, sizeof(tmp_dbscan.neighbors));

        uint32 nNeigh = 0;  // size of neighbor list
        uint32 idxN = 0;    // index for neighbors
        uint32 nb;          // neighbour

        if (n >= (npts - nhcells)) // if it's a lower horizontal border point
        {
          tmp_dbscan.neighbors[nNeigh++] = n + 1;
        }
        else if ((n % nhcells) == 0) // if it's a vertical border point on the left side
        {
          tmp_dbscan.neighbors[nNeigh++] = n + 1;
          tmp_dbscan.neighbors[nNeigh++] = n + nhcells;
          tmp_dbscan.neighbors[nNeigh++] = n + nhcells + 1;
        }
        else if ((n % nhcells) == (nhcells - 1)) // if it's a vertical border point on the right side
        {
          tmp_dbscan.neighbors[nNeigh++] = n + nhcells - 1;
          tmp_dbscan.neighbors[nNeigh++] = n + nhcells;
        }
        else
        {
          tmp_dbscan.neighbors[nNeigh++] = n + 1;
          tmp_dbscan.neighbors[nNeigh++] = n + nhcells - 1;
          tmp_dbscan.neighbors[nNeigh++] = n + nhcells;
          tmp_dbscan.neighbors[nNeigh++] = n + nhcells + 1;
        }

        while (idxN < nNeigh) // nNeigh can increase within this loop
        {
          nb = tmp_dbscan.neighbors[idxN];

          if (tmp_dbscan.Pvisit[nb] == 0) // if this neighbor has not been visited
          {
            tmp_dbscan.Pvisit[nb] = 1; // mark it as visited
            if (grid.a_cellcount[nb] >= ui_minPointsCluster)
            {
              if (nb == (npts - 1)) // last cell
              {
                // do nothing
              }
              else if (nb >= (npts - nhcells)) // if it's a horizontal border point
              {
                tmp_dbscan.neighbors[nNeigh++] = nb + 1;
              }
              else if ((nb % nhcells) == 0) // if it's a vertical border point on the left side
              {
                tmp_dbscan.neighbors[nNeigh++] = nb + 1;
                tmp_dbscan.neighbors[nNeigh++] = nb + nhcells;
                tmp_dbscan.neighbors[nNeigh++] = nb + nhcells + 1;
              }
              else if ((nb % nhcells) == (nhcells - 1)) // if it's a vertical border point on the right side
              {
                tmp_dbscan.neighbors[nNeigh++] = nb + nhcells - 1;
                tmp_dbscan.neighbors[nNeigh++] = nb + nhcells;
              }
              else
              {
                tmp_dbscan.neighbors[nNeigh++] = nb + 1;
                tmp_dbscan.neighbors[nNeigh++] = nb + nhcells - 1;
                tmp_dbscan.neighbors[nNeigh++] = nb + nhcells;
                tmp_dbscan.neighbors[nNeigh++] = nb + nhcells + 1;
              }
              s_clusterList.as_clusters[nClu].ui_numPoints++;
              s_clusterList.a_grdIndex[nCells] = nb;
              nCells++;
            }
          }
          idxN++;
        }
        nClu++;
      }
    }
  }
  s_clusterList.ui_numClusters = nClu;
}

// ---------------------------------------
// Grid-Sweep = lightning fast clustering!
// ---------------------------------------
void gridSweep()
{
  s_Grid_t         grid;
  s_ClusterList_t  s_clusterList;           // clusters of grid indices
  const uint32     ui_minPointsCluster = 1; // set to 1 if coarsegrid is grid!!
  s_DBSCAN_TMP_t   tmp_dbscan;

  memset(&grid, 0, sizeof(s_Grid_t));
  memset(&s_clusterList, 0, sizeof(s_ClusterList_t));
  memset(&tmp_dbscan, 0, sizeof(s_DBSCAN_TMP_t));

  for (int i = 0; i < FPA_SIZE; i++)
  {
    if (FPA[i] > 155)
      grid.a_cellcount[i] = 1;
  }
  grid.b_cells_square = true;
  grid.ui_celldim = 1;
  grid.ui_ncell = FPA_SIZE;

  gridNeighbours(
    grid,
    ui_minPointsCluster,
    tmp_dbscan, // <-- 2do, erst dort!!
    s_clusterList);

  char buf[55];
  sprintf(buf, "%d clusters found", MAXCLUSTER, s_clusterList.ui_numClusters);
  int msgboxID = MessageBox(
    NULL,
    buf,
    "gridsweep",
    MB_ICONINFORMATION | MB_OK
  );

  uint32 i, j, Nc, col, row;
  Nc = s_clusterList.ui_numClusters;

  // wrap up: i) color the pixel according to found clusters
  for (i = 0; i < Nc; ++i)
  {
    s_clusterList.as_clusters[i].ui_id = s_clusterList.as_clusters[i].ui_id;
    uint32 iCoarseStart = s_clusterList.as_clusters[i].ui_startIndex;
    uint32 iCoarseCells = s_clusterList.as_clusters[i].ui_numPoints;
    for (j = iCoarseStart; j < (iCoarseStart + iCoarseCells); ++j)
    {
      uint32 jj = s_clusterList.a_grdIndex[j];
      row = jj / TMP_GRID_DIMENSION_U;
      col = jj % TMP_GRID_DIMENSION_U;

      char color_ = s_clusterList.as_clusters[i].ui_id;
      zweidee::point pt = zweidee::point(col, row);
      zweidee::colrgb color = colcluster[color_];
      zweidee::fbuf2d.setpixel(zweidee::data, pt.x, CLUSTER_GUI_ROW1 + pt.y, color.r, color.g, color.b);
    }
  }

  // map to texture --> shift to zweidee.h
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);   // hier gibt es Schwierigkeiten mit .bmp,
}


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
      if (bCluster)
      {
//        kmeans_merge();
        
        gridSweep();

        bCluster = false;
      }

      if (bRefill)
      {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, zweidee::fbuf2d.width, zweidee::fbuf2d.height, 0, GL_BGR, GL_UNSIGNED_BYTE, zweidee::data);   // hier gibt es Schwierigkeiten mit .bmp,
        bRefill = false;
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


void fillData()
{
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
    int x = rand() % FPA_WIDTH - 1;
    int y = rand() % FPA_HEIGHT - 1;
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
      bCluster = true;
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
    case 70: // F-Fill
      fillData();
      bRefill = true;
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
