// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_CONST_H
#define COMMON_CONST_H

namespace consts {

#ifdef NDEBUG
  // Should python code emit thinking ops
  const int debug_thinking = 0;
  // Debug level for python code
  const int debug_level = 0;

  // Scale factor for time. The may become non-constant
  const float time_multiplier = 1.0;
  // In Real time how many seconds between ticks;
  const float basic_tick = time_multiplier * 3.0;
  // Highest possible person velocity
  const float base_velocity_coefficient = 5.0;
  const float base_velocity = base_velocity_coefficient/time_multiplier;
  // Square versions of above, to avoid square roots in calculations
  const float square_basic_tick = basic_tick * basic_tick;
  const float square_base_velocity = base_velocity * base_velocity;

  // Are ranges for broadcast enabled.
  const bool enable_ranges = true;
  // Range of broadcast sight ops
  const float sight_range = 26.0;
  // Range of broadcast sound ops
  const float hearing_range = 13.0;
  // Can things be omnipresent
  const bool enable_omnipresence = false;

#else // NDEBUG

// When we are doing a debug build, the constants are not constant

  extern int debug_thinking;
  extern int debug_level;
  extern float time_multiplier;
  extern float basic_tick;
  extern float base_velocity_coefficient;
  extern float base_velocity;
  extern float square_basic_tick;
  extern float square_base_velocity;
  extern float sight_range;
  extern float hearing_range;
  extern bool enable_ranges;
  extern bool enable_omnipresence;

#endif // NDEBUG

  // Admin password used by admin account
  extern const char * defaultAdminPasswordHash;
  // Id of root world entity
  extern const char * rootWorldId;
  // Version of the software we are running
  extern const char * version;
  // Time this server was built
  extern const char * buildTime;
  extern const char * buildDate;

}

#endif // COMMON_CONST_H
