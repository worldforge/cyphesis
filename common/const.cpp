// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "const.h"

namespace consts {

  // FIXME
  // Admin password used by admin account
  const char * defaultAdminPasswordHash = "247E9405E40979403510799CBBFF88BD";
  // Id of root world entity
  // const char * rootWorldId = "world_0";
  const char * rootWorldId = "0";
  // Version of the software we are running
  const char * version = VERSION;
  // Time this server was built
  const char * buildTime = __TIME__;
  const char * buildDate = __DATE__;


#ifndef NDEBUG

  // Should python code emit thinking ops
  int debug_thinking = 0;
  // Debug level for python code
  int debug_level = 0;

  // Scale factor for time. The may become non-constant
  double time_multiplier = 1.0;
  // In Real time how many seconds between ticks;
  double basic_tick = time_multiplier * 3.0;
  // Highest possible person velocity
  double base_velocity_coefficient = 5.0;
  double base_velocity = base_velocity_coefficient/time_multiplier;
  // Square versions of above, to avoid square roots in calculations
  double square_basic_tick = basic_tick * basic_tick;
  double square_base_velocity = base_velocity * base_velocity;

  // Are ranges for broadcast enabled.
  bool enable_ranges = true;
  // Range of broadcast sight ops
  double sight_range = 26.0;
  // Range of broadcast sound ops
  double hearing_range = 13.0;
  // Can things be omnipresent
  bool enable_omnipresence = false;

#endif

}
