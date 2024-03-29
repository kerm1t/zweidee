#pragma once

#include "zweidee.h"
#include "game.h"

namespace pitstop
{
  class CPitstop : public game::CGame
  {
  public:
    // ------------ Specific Variables ------------
    int init();
    int doit(unsigned char * data);
    int left();
    int right();
    int fire();
  private:
  };
}
