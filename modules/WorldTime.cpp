// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "WorldTime.h"

#include "common/debug.h"

#include <iostream>

#include <list>
#include <map>
#include <string>
#include <stdio.h>

//timedata time2type(const std::string & t) {
    //return timeInfo[t][1];
//}


// time2importance=time2type;

static const bool debug_flag = false;

static inline double minutes(const double & n) { return 60.0*n; }
static inline double hours(const double & n) { return minutes(60)*n; }
static inline double days(const double & n) { return hours(24)*n; }
static inline double months(const double & n) { return days(30)*n; }
static inline double years(const double & n) { return months(12)*n; }

inline const std::string seconds2string(double seconds) {
    char buffer[ 100 ];
    snprintf( buffer, 100, "%f", seconds );
    return buffer;
}

void WorldTime::initTimeInfo()
{
    m_timeInfo["always"] = Period(crange(0,12), "seasonal");
    m_timeInfo["spring"] = Period(crange(2,4), "seasonal");
    m_timeInfo["summer"] = Period(crange(5,7), "seasonal");
    m_timeInfo["autumn"] = Period(crange(8,10), "seasonal");
    Range winter = crange(0,1);
    winter.push_back(11);
    m_timeInfo["winter"] = Period(winter, "seasonal");
    m_timeInfo["dawn"] = Period(Range(1,8), "daily");
    m_timeInfo["midday"] = Period(Range(1,12), "daily");
    m_timeInfo["evening"] = Period(Range(1,20), "daily");
    Range night = crange(0,7);
    night.push_front(23); night.push_front(22); night.push_front(21);
    m_timeInfo["night"] = Period(night,"daily");
    m_timeInfo["day"] = Period(crange(9,18), "daily");
    m_timeInfo["morning"] = Period(crange(9,11), "daily");
    m_timeInfo["afternoon"] = Period(crange(13,18), "daily");
    m_timeInfo["now"] = Period(crange(0,23), "daily");
    std::list<std::string> seasons(1, "summer");
    seasons.push_back("autumn");
    seasons.push_back("winter");
    seasons.push_back("spring");
    std::list<std::string>::const_iterator I;
    for(I = seasons.begin(); I != seasons.end(); I++) {
        Range::const_iterator J;
        Range & months = m_timeInfo[*I].first;
        for(J = months.begin(); J != months.end(); J++) {
            m_monthToSeason[*J] = *I;
        }
    }
}


//WorldTime::WorldTime(char * date_time="1-1-1 0:0:0")
//{
    //// time=DateTime(date_time);
//}

std::string WorldTime::operator[](const std::string & name)
{
    if (name == "season") {
        SeasonInfo::const_iterator I = m_monthToSeason.find(m_time.month());
        if (I != m_monthToSeason.end()) {
            return I->second;
        }
    }
    return "";
}

bool WorldTime::operator==(const WorldTime & other) const
{
    return m_time == other.m_time;
}

bool WorldTime::operator==(const std::string & when) const
{
    debug(std::cout << "Checking whether it is " << when << " when the date is "
                    << m_time << std::endl << std::flush;);
            
    TimeInfo::const_iterator I = m_timeInfo.find(when);
    if (I == m_timeInfo.end()) {
        return false;
    }
    int check;
    if (I->second.second == "seasonal") {
        check = m_time.month();
    } else if (I->second.second == "daily") {
        check = m_time.hour();
    } else {
        return false;
    }
    std::list<int>::const_iterator J;
    for(J = I->second.first.begin(); J != I->second.first.end(); J++) {
        if (check == *J) {
            return true;
        }
    }
    return false;
}
