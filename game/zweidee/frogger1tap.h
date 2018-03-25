#pragma once

#include "proj_render.h"

#include "2dframebuf.hpp"

#include "game.h"

namespace frogger1tap
{
#define PLAYFIELD_H    FBUF2D_HEIGHT

#define N_ZEILEN       16                          // muss eine gerade Anzahl sein, sonst schlägt der "move%1" fehl
#define N_OBSTACLES     2                          // pro Zeile
#define FROG_Y_INIT    PLAYFIELD_H/2
#define LANE_HEIGHT    (int)(PLAYFIELD_H/N_ZEILEN)
#define OBJ_HEIGHT     LANE_HEIGHT-5               // obstacle + frog

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
    int lane;
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
    unsigned char ystart = 0;

    void draw_obst(unsigned char * data);
    void draw_frog(unsigned char * data);
  public:

    int init();
    int doit(unsigned char * data);
    int fire(); // == Tap()
    int left(); // virtual function, needs to be implemented, otherwise class is abstract
    int right(); // virtual function, needs to be implemented, otherwise class is abstract
  };
}
