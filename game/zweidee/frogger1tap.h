#pragma once

#include "proj_render.h"

#include "2dframebuf.hpp"

#include "game.h"

namespace frogger1tap
{
#define N_ZEILEN     6 // muss eine gerade Anzahl sein, sonst schlägt der "move%1" fehl
#define N_OBSTACLES  2
#define FROG_Y_INIT 35

  enum frogstate { start, floating, finished };
  enum obst_type { tree, croco };
  class CObstacle;

  // frog -> player
  class CFrog
  {
  public:
    frogstate state;
//    const CObstacle & obst; // <-- C2280 CFrogger is a deleted function
    CObstacle * p_obst;
    int x; // pos. auf obstacle
    int y_dummy;
    void init();
  };
  // background
  class CBackground
  {

  };
  // obstacles (enemies / objects)
  class CObstacle
  {
  public:
    obst_type type;
    int x;
    int w;
    int r; // x+w
  };

  class CFrogger : public game::CGame
  {
    CFrog     frog;
    CObstacle obst[N_ZEILEN][N_OBSTACLES];
    unsigned char rot = 0;

  public:
    void draw_obst(unsigned char * data);

    int init();
    int doit(unsigned char * data);
    int fire(); // == Tap()
    int left(); // virtual function, needs to be implemented, otherwise class is abstract
    int right(); // virtual function, needs to be implemented, otherwise class is abstract
  };
}
