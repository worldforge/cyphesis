// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MODULES_WORLD_TIME_H
#define MODULES_WORLD_TIME_H

#include <modules/DateTime.h>

// timedata time2type(const std::string & t);

class WorldTime {
    typedef std::list<int> range;
    typedef std::pair<range, std::string> period;
    typedef std::map<std::string, period> time_info_t;

    //DateTime time;
    //string/int month;
    double secs;

    time_info_t	timeInfo;
    std::map<int, std::string> monthToSeason;

    range crange(int begin, int end) {
        range ret;
        for(int i = begin; i <= end; i++) {
            ret.push_back(i);
        }
        return ret;
    }

    void initTimeInfo();
  public:
    WorldTime(double seconds) : secs(seconds) {
        initTimeInfo();
    }
    double seconds() { return secs; }
    //WorldTime(char * date_time);
    const std::string & operator[](const std::string & name);
    //std::string & __repr__();
    //std::string & __str__();
    //bool operator==(const WorldTime & other);
};

const std::string & seconds2string(double seconds);

#endif // MODULES_WORLD_TIME_H
