#pragma once

#include "zweidee.h"
#include "game.h"

namespace galaga
{
  const glm::vec3 cols[6] = { glm::vec3(0,0,0),       // none
                              glm::vec3(155,155,255), // light-blue
                              glm::vec3(110,110,255), // mid-blue
                              glm::vec3(255,  0,  0), // red
                              glm::vec3(80,  80,255), // darker blue
                              glm::vec3(190,190,255)  // very light blue
                            };

  const uint8 spacecraft_w = 7;
  const uint8 spacecraft_h = 3;
  // Anton's proposal
  const uint8 aSpacecraft[spacecraft_w*spacecraft_h] = {
    0,0,1,1,1,0,0,
    2,0,1,3,1,0,2,
    2,4,1,1,1,4,2
  };

  const uint8 spacecraft_turnleft_w = 5;
  const uint8 spacecraft_turnleft_h = 3;
  const uint8 aSpacecraft_turnleft[spacecraft_turnleft_w*spacecraft_turnleft_h] = {
    0,1,1,1,0,
    2,1,3,1,5,
    2,1,1,1,5
  };

  const uint8 spacecraft_turnright_w = 5;
  const uint8 spacecraft_turnright_h = 3;
  const uint8 aSpacecraft_turnright[spacecraft_turnright_w*spacecraft_turnright_h] = {
    0,1,1,1,0,
    5,1,3,1,2,
    5,1,1,1,2
  };

  const uint8 enemy_w = 3;
  const uint8 enemy_h = 2;
  const uint8 aEnemy[enemy_w*enemy_h] = {
    3,3,3,
    0,3,0
  };

  const uint8 gameover_w = 17;
  const uint8 gameover_h = 9;
  const uint8 aGameover[gameover_w*gameover_h] = {
    1,1,1,0,1,1,1,0,1,0,0,0,1,0,1,1,1, // Game
    1,0,0,0,1,0,1,0,1,1,0,1,1,0,1,0,0,
    1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,0,
    1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,0,0,1,0,1,0,1,1,1,0,1,1,1,0, // Over
    1,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,1,
    1,0,0,1,0,1,1,1,0,1,1,0,0,1,1,1,0,
    0,1,1,0,0,0,1,0,0,1,1,1,0,1,0,1,1
  };

  typedef struct
  {
    uint32 x; // left
    uint32 y; // top
    uint32 w;
    uint32 h;
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

  namespace spacecraft {
    enum spacecraft_state {normal, explode};
  }
  namespace enemy {
    enum enemy_state {on, off, explode};
  }

  typedef struct
  {
    enemy::enemy_state state;
    uint8 explode_counter;
    rect box;
  } sEnemy;

  typedef struct
  {
    rect box;
    spacecraft::spacecraft_state state;
    uint8 explode_counter;
    uint32 dir;   // 0=fwd,1=lft,2=right
    uint32 lives; // 4
  } sSpaceCraft;

  bool DoBoxesIntersect(rect a, rect b);

  class CGalaga : public game::CGame
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
    int draw_starfield_vert(unsigned char * data); // 2do: replace with random starfield

    int draw_obj(const rect r, const unsigned char * obj, unsigned char * data);
        
    int draw_spacecraft(unsigned char * data);
    int draw_spacecraft_turnleft(unsigned char * data);
    int draw_spacecraft_turnright(unsigned char * data);
    int draw_spacecraft_explode(unsigned char * data);
    
    int draw_shots(unsigned char * data);
    
    int draw_enemy_explosion(const unsigned int i, unsigned char * data);
    int draw_enemies(unsigned char * data);
    
    int draw_gameover(unsigned char * data);

    int move_shots();
    int move_enemies();
    int collisioncheck();
  };
}
