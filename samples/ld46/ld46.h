#pragma once
#pragma once

#include "zweidee.h"
#include "game.h"

namespace ld46
{
  static int lvl_w = 60; // 32
  static int lvl_h = 44; // 24
  static int lvl_size = lvl_w*lvl_h;
  static char * lvl;

  const glm::vec3 cols[6] = {
    glm::vec3(0,0,0),       // none
    glm::vec3(155,155,255), // light-blue
    glm::vec3(110,110,255), // mid-blue
    glm::vec3(255,  0,  0), // red
    glm::vec3(80,  80,255), // darker blue
    glm::vec3(190,190,255)  // very light blue
  };

  typedef struct // --> game.h
  {
    uint32 x; // left
    uint32 y; // top
    uint32 w;
    uint32 h;
  } rect;

  namespace hero { // --> game.h
    enum hero_state { normal, explode };
  }
  namespace enemy { // --> game.h
    enum enemy_state { on, off, explode };
  }

  typedef struct // --> game.h
  {
    rect box;
    hero::hero_state state;
    uint8 explode_counter;
    uint32 dir;   // 0=fwd,1=lft,2=right,3=up,4=down
    uint32 lives; // 4
  } sHero;

  class CLD46 : public game::CGame
  {
  public:
    // ------------ Specific Variables ------------
    sHero hero;

    int init();
    int doit(unsigned char * data);
    int left();
    int right();
    int up();
    int down();
    int fire();
  private:
    int draw_hero(unsigned char * data);
  };
}
