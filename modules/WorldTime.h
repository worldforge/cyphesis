// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef MODULES_WORLD_TIME_H
#define MODULES_WORLD_TIME_H

#include "DateTime.h"

#include <list>
#include <map>

class WorldTime {
  private:
    typedef std::list<int> Range;
    typedef std::pair<Range, std::string> Period;
    typedef std::map<std::string, Period> TimeInfo;
    typedef std::map<unsigned int, std::string> SeasonInfo;

    DateTime m_time;
    TimeInfo m_timeInfo;
    SeasonInfo m_monthToSeason;

    Range crange(int begin, int end) {
        Range ret;
        for(int i = begin; i <= end; ++i) {
            ret.push_back(i);
        }
        return ret;
    }

    void initTimeInfo();
  public:
    explicit WorldTime(int scnds) : m_time(scnds) {
        initTimeInfo();
    }
    WorldTime() : m_time(0) {
        initTimeInfo();
    }
    double seconds() const { return m_time.seconds(); }
    void update(int secs) { m_time.update(secs); }
    std::string operator[](const std::string & name) const;
    bool operator==(const WorldTime & other) const;
    bool operator==(const std::string & when) const;
};

#endif // MODULES_WORLD_TIME_H
