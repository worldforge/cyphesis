// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_CONST_H
#define COMMON_CONST_H

// #define set(_name, _val) _name = _val

namespace consts {

  extern int debug_level;
  extern int debug_thinking;

  extern double fzero;

  extern double time_multiplier;
  extern double basic_tick;
  extern double base_velocity_coefficient;
  extern double base_velocity;
  extern int day_in_seconds;

  extern double sight_range;
  extern double hearing_range;
  extern double collision_range;
  extern int enable_ranges;

}

#endif /* COMMON_CONST_H */
