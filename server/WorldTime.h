// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef WORLD_TIME_H
#define WORLD_TIME_H

#include <modules/DateTime.h>

#include <list>

// timedata time2type(const string & t);

class WorldTime {
  private:
    typedef std::list<int> range;
    typedef std::pair<range, std::string> period;
    typedef std::map<std::string, period> time_info_t;

    DateTime time;

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
    WorldTime(double scnds) : time((int)scnds) {
        initTimeInfo();
    }
    WorldTime() : time(0) {
        initTimeInfo();
    }
    double seconds() { return time.seconds(); }
    void update(double secs) { time.update((int)secs); }
    //WorldTime(char * date_time);
    std::string operator[](const std::string & name);
    //string & __repr__();
    //string & __str__();
    bool operator==(const WorldTime & other) const;
    bool operator==(const string & when) const;
};

const std::string & seconds2string(double seconds);

#endif // WORLD_TIME_H
