// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "DateTime.h"

// date_pat=re.compile("^|[-:]|\s+");

unsigned int DateTime::m_spm = 60; // seconds per minute
unsigned int DateTime::m_mph = 60; // minutes per hour
unsigned int DateTime::m_hpd = 24; // hours per day
unsigned int DateTime::m_dpm = 28; // days per month
unsigned int DateTime::m_mpy = 12; // months per year

DateTime::DateTime(char * date_time)
{
	// extract numbers from string
        //(DateTime::year,DateTime::month,DateTime::day,;
         //DateTime::hour,DateTime::minute,DateTime::second)=\;
         //map(float,date_pat.split(date_time));
}

DateTime::DateTime(int yr, int mn, int da, int hr, int mt, int sc) :
  m_second(sc), m_minute(mt), m_hour(hr), m_day(da), m_month(mn), m_year(yr)
{
}

double DateTime::seconds()
{
    return m_second+
           m_minute*m_spm+
           m_hour*m_spm*m_mph+
           (m_day-1)*m_spm*m_mph*m_hpd+
           (m_month-1)*m_spm*m_mph*m_hpd*m_dpm+
           m_year*m_spm*m_mph*m_hpd*m_dpm*m_mpy;
}

string DateTime::asString()
{
    //Convert date into string
    //return "%04i-%02i-%02i %02i:%02i:%04.1f" %
         //(DateTime::year,DateTime::month,DateTime::day,;
            //DateTime::hour,DateTime::minute,DateTime::second);
}
