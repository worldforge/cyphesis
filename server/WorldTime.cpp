// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <list>
#include <map>
#include <string>

#include "WorldTime.h"


//timedata time2type(const string & t) {
    //return time_info[t][1];
//}


// time2importance=time2type;


static inline double minutes(const double & n) { return 60.0*n; }
static inline double hours(const double & n) { return minutes(60)*n; }
static inline double days(const double & n) { return hours(24)*n; }
static inline double months(const double & n) { return days(30)*n; }
static inline double years(const double & n) { return months(12)*n; }

inline const string & seconds2string(double seconds) {
    // date_time=DateTime(seconds);
    // return str(date_time);
}

//WorldTime::WorldTime(char * date_time="1-1-1 0:0:0")
//{
    //// time=DateTime(date_time);
//}

const string & WorldTime::operator[](const string & name)
{
    //if (name=="season") {
        //return month2season[month];
    //}
    return "what";
}

#if 0
void WorldTime::__call__()
{
    return WorldTime::time;
}

double WorldTime::time()
{
    return time.seconds();
}

char * WorldTime::asString()
{
    return WorldTime::time.asString();
}

bool WorldTime::__cmp__(const WorldTime & other)
{
    if (time2type(other)=='seasonal') {
        if (WorldTime::time.month in time_info[other][0]) {
            return 0;
        } else {
            return 1;
        }
    }
    if (time2type(other)=='daily') {
        if (WorldTime::time.hour in time_info[other][0]) {
            return 0;
        } else {
            return 1;
        }
    }
    return 1;
}

dobule WorldTime::seconds()
{
    return WorldTime::time.seconds();
}

#endif
