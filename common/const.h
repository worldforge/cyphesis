#ifndef COMMON_CONST_H
#define COMMON_CONST_H

#define set(_name, _val) _name = _val

namespace consts {

  extern int debug_level;
  extern int debug_thinking;

  extern double fzero;

  extern int time_multiplier;
  extern int basic_tick;
  extern double base_velocity_coefficient;
  extern double base_velocity;
  extern int day_in_seconds;

  extern double sight_range;
  extern double hearing_range;
  extern double collision_range;
  extern int enable_ranges;

}

#endif /* COMMON_CONST_H */
