// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "common/const.h"

#include <string>

#include <cassert>

int main()
{
    int debug_thinking = consts::debug_thinking;
    int debug_level = consts::debug_level;

    float time_multiplier = consts::time_multiplier;
    assert(time_multiplier > 0);
    float basic_tick = consts::basic_tick;
    assert(basic_tick > 0);
    float base_velocity_coefficient = consts::base_velocity_coefficient;
    assert(base_velocity_coefficient > 0);
    float base_velocity = consts::base_velocity;
    assert(base_velocity > 0);
    float square_basic_tick = consts::square_basic_tick;
    assert(square_basic_tick > 0);
    float square_base_velocity = consts::square_base_velocity;
    assert(square_base_velocity > 0);
    float sight_range = consts::sight_range;
    assert(sight_range > 0);
    float square_sight_range = consts::square_sight_range;
    assert(square_sight_range > 0);
    float sight_factor = consts::sight_factor;
    assert(sight_factor > 0);
    float square_sight_factor = consts::square_sight_factor;
    assert(square_sight_factor > 0);
    float hearing_range = consts::hearing_range;
    assert(hearing_range > 0);
    float minSqrBoxSize = consts::minSqrBoxSize;
    assert(minSqrBoxSize > 0);
    
    bool enable_ranges = consts::enable_ranges;
    bool enable_omnipresence = consts::enable_omnipresence;
    bool enable_persistence = consts::enable_persistence;

    const char * defaultAdminPasswordHash = consts::defaultAdminPasswordHash;
    assert(defaultAdminPasswordHash != 0);
    std::string defaultAdminPasswordHashs = consts::defaultAdminPasswordHash;
    assert(defaultAdminPasswordHashs.size() > 6);

    const char * rootWorldId = consts::rootWorldId;
    assert(rootWorldId != 0);
    std::string rootWorldIds = consts::rootWorldId;
    assert(rootWorldIds.size() > 0);

    const char * version = consts::version;
    assert(version != 0);
    std::string versions = consts::version;
    assert(versions.size() > 0);

    const char * buildTime = consts::buildTime;
    assert(buildTime != 0);
    std::string buildTimes = consts::buildTime;
    assert(buildTimes.size() > 0);

    const char * buildDate = consts::buildDate;
    assert(buildDate != 0);
    std::string buildDates = consts::buildDate;
    assert(buildDates.size() > 0);

    return 0;
}
