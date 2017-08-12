#pragma once

#include "proj_render.h"

#include "2dframebuf.hpp"

namespace galaga
{
  typedef struct
  {
    unsigned int x;
    unsigned int y;
  } sPoint;

  typedef struct
  {
    bool on;
    unsigned int x;
    unsigned int y;
  } sShot;

  typedef struct
  {
    bool on;
    unsigned int x;
    unsigned int y;
  } sEnemy;

  class CGame
  {
  public:
    FrameBuf2D fbuf2d;

#define NUM_EPISODES 3
    char a_episode_len[NUM_EPISODES];
    bool a_episode_active[NUM_EPISODES];

    unsigned int iepisode;
    int iloopy;

///    CGame(FrameBuf2D & _fbuf2d) : fbuf2d(_fbuf2d)
///    {
///    };

    virtual int init() = 0;
    virtual int doit(unsigned char * data) = 0;
    // keys
    virtual int left()  = 0;
    virtual int right() = 0;
    virtual int fire()  = 0;
  };

  class CGalaga : public CGame
  {
  public:
    // ------------ Specific Variables ------------
    unsigned int xSpaceCraft;
    unsigned int SpacecraftDir;// 0=fwd,1=lft,2=right
    unsigned int SpacecraftNum;// 4
#define NUM_SHOTS  100
    sShot a_shots[NUM_SHOTS]; // ringbuffer
    char idx_shots;
#define NUM_ENEMIES  8
    sEnemy a_enemies[NUM_ENEMIES];

///    CGalaga() : CGame(stat_fbuf()){};
//    CGalaga(FrameBuf2D & _fbuf2d) : CGame(_fbuf2d){};

    int init();
    int doit(unsigned char * data);
    int left();
    int right();
    int fire();
  private:
///    FrameBuf2D stat_fbuf() { FrameBuf2D f = FrameBuf2D(); return f; };

    int draw_starfield_vert(unsigned char * data); // 2do: replace with random starfield
    //     xxx
    //     xxx
    //
    //     x   xx   x
    //     xxxxxxxxxx
    int draw_spacecraft(unsigned char * data);
    int draw_spacecraft_turnleft(unsigned char * data);
    int draw_spacecraft_turnright(unsigned char * data);
    int draw_shots(unsigned char * data);
    int draw_enemies(unsigned char * data);

    int move_shots();
    int move_enemies();
    int collisioncheck();
  };
}