// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MODULES_WORLD_TIME_H
#define MODULES_WORLD_TIME_H

#include "DateTime.h"

#include <list>
#include <map>

// timedata time2type(const std::string & t);

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
    double seconds() { return m_time.seconds(); }
    void update(int secs) { m_time.update(secs); }
    //explicit WorldTime(char * date_time);
    std::string operator[](const std::string & name);
    //std::string & __repr__();
    //std::string & __str__();
    bool operator==(const WorldTime & other) const;
    bool operator==(const std::string & when) const;
};

const std::string seconds2string(double seconds);

#endif // MODULES_WORLD_TIME_H
