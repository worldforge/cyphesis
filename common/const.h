// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_CONST_H
#define COMMON_CONST_H

// #define set(_name, _val) _name = _val

namespace consts {

  const int debug_level = 0;
  const int debug_thinking = 0;

  const double fzero = 1e-13; //float zero;

  const double time_multiplier = 1.0;
  // In Real time how many seconds between ticks;
  const double basic_tick = time_multiplier*3.0;
  const double base_velocity_coefficient = 5.0;
  const double base_velocity = base_velocity_coefficient/time_multiplier;
  const int day_in_seconds = 3600*24;


  const double sight_range = 10.0;
  const double hearing_range = 10.0;
  const double collision_range = 1.0;
  const bool enable_ranges = true;

}

#endif /* COMMON_CONST_H */
