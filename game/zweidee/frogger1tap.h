#pragma once

#include "zweidee.h"
#include "game.h"

namespace frogger1tap
{

#define PLAYFIELD_W         FBUF2D_WIDTH
#define PLAYFIELD_H         FBUF2D_HEIGHT

// Playfield:
#define PLAYFIELD_X_MIN     0//30     // nur für's zeichnen!
#define AREA_W              PLAYFIELD_W - PLAYFIELD_X_MIN
/*
   +---+-----------------+
   |   |    Area Head    |
   + s +-----------------+
   | t |                 |
   | a |  Area Floating  |
   | r |                 |
   + t +-----------------+
   | *)|    Area Foot    |
   +-.-+-----------------+
     *) start area is optional
*/
#define AREA_HEAD           20
#define AREA_FOOT           20
#define AREA_FLOATING       PLAYFIELD_H //- AREA_HEAD - AREA_FOOT // 280


// Area Floating:
#define N_ZEILEN            16                            // muss eine gerade Anzahl sein, sonst schlägt der "move%1" fehl
#define N_OBSTACLES          2                            // pro Zeile
#define LANE_HEIGHT         (int)(AREA_FLOATING/N_ZEILEN)
#define OBJ_HEIGHT          LANE_HEIGHT-5                 // obstacle + frog

#define FROG_Y_INIT         PLAYFIELD_H/2
#define FROG_W              OBJ_HEIGHT

  enum frogstate { start, floating, finished };
  enum obst_type { tree, croco, turtle };
  class CObstacle;

  // frog -> player
  class CFrog
  {
  public:
    frogstate state;
    CObstacle * p_obst;
    int x;
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
    unsigned int ystart = 0;

    void draw_area_start(unsigned char * data);
    void draw_obst(unsigned char * data);
    void draw_frog(unsigned char * data);
  public:

    int init();
    int doit(unsigned char * data);
    int fire();  // == Tap()
    int left();  // virtual function, needs to be implemented, otherwise class is abstract
    int right(); // virtual function, needs to be implemented, otherwise class is abstract
  };
}
