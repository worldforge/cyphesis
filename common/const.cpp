// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "const.h"

namespace consts {

  int debug_level = 0;
  int debug_thinking = 0;

  double fzero = 1e-13; //float zero;

  double time_multiplier = 1.0;
  // In Real time how many seconds between ticks;
  double basic_tick = time_multiplier*3.0;
  double base_velocity_coefficient = 5.0;
  double base_velocity = base_velocity_coefficient/time_multiplier;
  int day_in_seconds = 3600*24;


  double sight_range = 100.0;
  double hearing_range = 10.0;
  double collision_range = 1.0;
  int enable_ranges = 0;

}
