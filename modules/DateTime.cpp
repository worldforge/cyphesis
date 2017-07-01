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


#include "DateTime.h"

#include "common/globals.h"

// date_pat=re.compile("^|[-:]|\s+");

// unsigned int DateTime::m_spm = 60; // seconds per minute

// Acorn 1/3 time hardcoded for now. This means we keep 24 hours per
// day, and seconds are still the same length. Clocks would still look
// same too.
unsigned int DateTime::m_spm = SPM; // seconds per minute
unsigned int DateTime::m_mph = MPH; // minutes per hour
unsigned int DateTime::m_hpd = HPD; // hours per day
unsigned int DateTime::m_dpm = DPM; // days per month
unsigned int DateTime::m_mpy = MPY; // months per year

inline void DateTime::set(unsigned int t)
{
    m_second = t % m_spm;
    t /= m_spm;
    m_minute = t % m_mph;
    t /= m_mph;
    m_hour = t % m_hpd;
    t /= m_hpd;
    m_day = t % m_dpm;
    t /= m_dpm;
    m_month = t % m_mpy;
    t /= m_mpy;
    m_year = t;
    
}

DateTime::DateTime(char * date_time) :
  m_second(0), m_minute(0), m_hour(0), m_day(0), m_month(0), m_year(0)
{
    // Wouldnt it be better do make date_time a std::string directly ?
    std::string date( date_time );
    if( date.length() == 19 )
    {
        m_year = atoi( date.substr( 0, 4 ).c_str() );
	m_month = atoi( date.substr( 5, 2 ).c_str() );
	m_day = atoi( date.substr( 8, 2 ).c_str() );
	m_hour = atoi( date.substr( 11, 2 ).c_str() );
	m_minute = atoi( date.substr( 14, 2 ).c_str() );
	m_second = atoi( date.substr( 17, 2 ).c_str() );
    }
}

DateTime::DateTime(int t)
{
    set(t);
}


DateTime::DateTime(int yr, int mn, int da, int hr, int mt, int sc) :
  m_second(sc), m_minute(mt), m_hour(hr), m_day(da), m_month(mn), m_year(yr)
{
}

bool DateTime::isValid() const
{
    return ( m_second < m_spm ) &&
           ( m_minute < m_mph ) &&
           ( m_hour   < m_hpd ) &&
           ( m_day    < m_dpm ) &&
           ( m_month  < m_mpy );
}

int DateTime::seconds()
{
    return m_second+
           m_minute*m_spm+
           m_hour*m_spm*m_mph+
           (m_day-1)*m_spm*m_mph*m_hpd+
           (m_month-1)*m_spm*m_mph*m_hpd*m_dpm+
           m_year*m_spm*m_mph*m_hpd*m_dpm*m_mpy;
}

void DateTime::update(int t)
{
    set(t);
}

std::string DateTime::asString()
{
    //Convert date into string
    char buffer[ 100 ];
    snprintf( buffer, 100, "%04d-%02d-%02d %02d:%02d:%02d", m_year, m_month, m_day, m_hour, m_minute, m_second );
    return std::string( buffer );
}

bool DateTime::operator==( const DateTime & date ) const
{
    return ( m_year == date.m_year ) &&
           ( m_month == date.m_month ) &&
           ( m_day == date.m_day ) && 
           ( m_hour == date.m_hour ) &&
           ( m_minute == date.m_minute ) &&
           ( m_second == date.m_second );
}
