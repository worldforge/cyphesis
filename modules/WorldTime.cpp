// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <list>
#include <map>
#include <string>

#include "WorldTime.h"


//timedata time2type(const std::string & t) {
    //return timeInfo[t][1];
//}


// time2importance=time2type;


static inline double minutes(const double & n) { return 60.0*n; }
static inline double hours(const double & n) { return minutes(60)*n; }
static inline double days(const double & n) { return hours(24)*n; }
static inline double months(const double & n) { return days(30)*n; }
static inline double years(const double & n) { return months(12)*n; }

inline const std::string & seconds2string(double seconds) {
    // date_time=DateTime(seconds);
    // return str(date_time);
}

void WorldTime::initTimeInfo() {
    timeInfo["always"] = period(crange(1,13), "seasonal");
    timeInfo["spring"] = period(crange(3,5), "seasonal");
    timeInfo["summer"] = period(range(6,8), "seasonal");
    timeInfo["autumn"] = period(range(9,11), "seasonal");
    range winter = crange(1,2);
    winter.push_back(12);
    timeInfo["winter"] = period(winter, "seasonal");
    timeInfo["morning"] = period(range(1,8), "daily");
    timeInfo["midday"] = period(range(1,12), "daily");
    timeInfo["evening"] = period(range(1,20), "daily");
    range night = crange(0,7);
    night.push_front(23); night.push_front(22); night.push_front(21);
    timeInfo["night"] = period(night,"daily");
    timeInfo["now"] = period(crange(0,23), "daily");
    std::list<std::string> seasons(1, "summer");
    seasons.push_back("autumn");
    seasons.push_back("winter");
    seasons.push_back("spring");
    std::list<std::string>::const_iterator I;
    for(I = seasons.begin(); I != seasons.end(); I++) {
        range::const_iterator J;
        range & months = timeInfo[*I].first;
        for(J = months.begin(); J != months.end(); J++) {
            monthToSeason[*J] = *I;
        }
    }
}


//WorldTime::WorldTime(char * date_time="1-1-1 0:0:0")
//{
    //// time=DateTime(date_time);
//}

const std::string & WorldTime::operator[](const std::string & name)
{
    //if (name=="season") {
        //return month2season[month];
    //}
    return "what";
}
