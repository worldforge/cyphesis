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

    time_info_t	time_info;
    std::map<int, string> month2season;

    range crange(int begin, int end) {
        range ret;
        for(int i = begin; i <= end; i++) {
            ret.push_back(i);
        }
        return ret;
    }

    void init_time_info() {
        time_info["always"] = period(crange(1,13), "seasonal");
        time_info["spring"] = period(crange(3,5), "seasonal");
        time_info["summer"] = period(range(6,8), "seasonal");
        time_info["autumn"] = period(range(9,11), "seasonal");
        range winter = crange(1,2);
        winter.push_back(12);
        time_info["winter"] = period(winter, "seasonal");
        time_info["morning"] = period(range(1,8), "daily");
        time_info["midday"] = period(range(1,12), "daily");
        time_info["evening"] = period(range(1,20), "daily");
        range night = crange(0,7);
        night.push_front(23); night.push_front(22); night.push_front(21);
        time_info["night"] = period(night,"daily");
        time_info["now"] = period(crange(0,23), "daily");
        std::list<string> seasons(1, "summer");
        seasons.push_back("autumn");
        seasons.push_back("winter");
        seasons.push_back("spring");
        std::list<string>::const_iterator I;
        for(I = seasons.begin(); I != seasons.end(); I++) {
            range::const_iterator J;
            range & months = time_info[*I].first;
            for(J = months.begin(); J != months.end(); J++) {
                month2season[*J] = *I;
            }
        }
    }
  public:
    WorldTime(double seconds) : secs(seconds) {
        init_time_info();
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
