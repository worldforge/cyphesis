// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_CONST_H
#define COMMON_CONST_H

namespace consts {
  const int debug_thinking = 0;
  const int debug_level = 0;

  const double time_multiplier = 1.0;
  // In Real time how many seconds between ticks;
  const double basic_tick = time_multiplier*3.0;
  const double base_velocity_coefficient = 5.0;
  const double base_velocity = base_velocity_coefficient/time_multiplier;
  const double square_basic_tick = basic_tick * basic_tick;
  const double square_base_velocity = base_velocity * base_velocity;

  const double sight_range = 26.0;
  const double hearing_range = 13.0;
  const bool enable_ranges = true;

  const char * const defaultAdminPassword = "zjvspoehrgopes";

}

#endif // COMMON_CONST_H
