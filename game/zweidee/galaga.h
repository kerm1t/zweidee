#pragma once

#include "proj_render.h"

#include "2dframebuf.hpp"

namespace galaga
{
  const glm::vec3 cols[6] = { glm::vec3(0,0,0), // none
                              glm::vec3(155,155,255), // light-blue
                              glm::vec3(110,110,255), // mid-blue
                              glm::vec3(255,  0,  0), // red
                              glm::vec3(80,  80,255), // darker blue
                              glm::vec3(190,190,255)  // very light blue
                            };

  const unsigned char spacecraft_w = 7;
  const unsigned char spacecraft_h = 3;
  // Anton's proposal
  const unsigned char aSpacecraft[spacecraft_w*spacecraft_h] = {
    0,0,1,1,1,0,0,
    2,0,1,3,1,0,2,
    2,4,1,1,1,4,2
  };

  const unsigned char spacecraft_turnleft_w = 5;
  const unsigned char spacecraft_turnleft_h = 3;
  const unsigned char aSpacecraft_turnleft[spacecraft_turnleft_w*spacecraft_turnleft_h] = {
    0,1,1,1,0,
    2,1,3,1,5,
    2,1,1,1,5
  };

  const unsigned char spacecraft_turnright_w = 5;
  const unsigned char spacecraft_turnright_h = 3;
  const unsigned char aSpacecraft_turnright[spacecraft_turnright_w*spacecraft_turnright_h] = {
    0,1,1,1,0,
    5,1,3,1,2,
    5,1,1,1,2
  };

  const unsigned char enemy_w = 3;
  const unsigned char enemy_h = 2;
  const unsigned char aEnemy[enemy_w*enemy_h] = {
    3,3,3,
    0,3,0
  };

  typedef struct
  {
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
  } rect;

  typedef struct
  {
    rect box;
  } sPoint;

  typedef struct
  {
    bool on;
    rect box;
  } sShot;

  typedef struct
  {
    bool on;
    rect box;
  } sEnemy;

  enum spacecraft_state {normal, explode};

  typedef struct
  {
    rect box;
    spacecraft_state state;
    unsigned char explode_counter;
    unsigned int dir; // 0=fwd,1=lft,2=right
    unsigned int num; // 4
  } sSpaceCraft;

  bool DoBoxesIntersect(rect a, rect b);

  class CGame
  {
  public:
    FrameBuf2D fbuf2d;

#define NUM_EPISODES 3
    char a_episode_len[NUM_EPISODES];
    bool a_episode_active[NUM_EPISODES];

    unsigned int iepisode;
    int iloopy;

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
    sSpaceCraft spacecraft;

#define NUM_SHOTS  100
    sShot a_shots[NUM_SHOTS]; // ringbuffer
    char idx_shots;

#define NUM_ENEMIES  8
    sEnemy a_enemies[NUM_ENEMIES];

    int init();
    int doit(unsigned char * data);
    int left();
    int right();
    int fire();
  private:
    int draw_obj(const rect r, const unsigned char * obj, unsigned char * data);
    int draw_starfield_vert(unsigned char * data); // 2do: replace with random starfield
    int draw_spacecraft(unsigned char * data);
    int draw_spacecraft_turnleft(unsigned char * data);
    int draw_spacecraft_turnright(unsigned char * data);
    int draw_spacecraft_explode(unsigned char * data);
    int draw_shots(unsigned char * data);
    int draw_enemies(unsigned char * data);

    int move_shots();
    int move_enemies();
    int collisioncheck();
  };
}