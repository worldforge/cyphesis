// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef WORLD_TIME_H
#define WORLD_TIME_H

#include <modules/DateTime.h>

// timedata time2type(const string & t);

class WorldTime {
    typedef std::list<int> range;
    typedef std::pair<range, std::string> period;
    typedef std::map<string, period> time_info_t;

    //DateTime time;
    //string/int month;
    double secs;

    time_info_t	timeInfo;
    std::map<int, string> monthToSeason;

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
    const string & operator[](const string & name);
    //string & __repr__();
    //string & __str__();
    //bool operator==(const WorldTime & other);
};

const string & seconds2string(double seconds);

#endif /* WORLD_TIME_H */
