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
  const double time_multiplier = 1.0;
  // In Real time how many seconds between ticks;
  const double basic_tick = time_multiplier * 3.0;
  // Highest possible person velocity
  const double base_velocity_coefficient = 5.0;
  const double base_velocity = base_velocity_coefficient/time_multiplier;
  // Square versions of above, to avoid square roots in calculations
  const double square_basic_tick = basic_tick * basic_tick;
  const double square_base_velocity = base_velocity * base_velocity;

  // Are ranges for broadcast enabled.
  const bool enable_ranges = true;
  // Range of broadcast sight ops
  const double sight_range = 26.0;
  // Range of broadcast sound ops
  const double hearing_range = 13.0;
  // Can things be omnipresent
  const bool enable_omnipresence = false;

#else // NDEBUG

// When we are doing a debug build, the constants are not constant

  extern int debug_thinking;
  extern int debug_level;
  extern double time_multiplier;
  extern double basic_tick;
  extern double base_velocity_coefficient;
  extern double base_velocity;
  extern double square_basic_tick;
  extern double square_base_velocity;
  extern double sight_range;
  extern double hearing_range;
  extern bool enable_ranges;
  extern bool enable_omnipresence;

#endif // NDEBUG

  // Admin password used by admin account
  extern const char * defaultAdminPassword;
  // Id of root world entity
  extern const char * rootWorldId;
  // Version of the software we are running
  extern const char * version;
  // Time this server was built
  extern const char * buildTime;
  extern const char * buildDate;

}

#endif // COMMON_CONST_H
