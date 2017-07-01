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


#include "modules/DateTime.h"

unsigned int DateTime::m_spm = SPM; // seconds per minute
unsigned int DateTime::m_mph = MPH; // minutes per hour
unsigned int DateTime::m_hpd = HPD; // hours per day
unsigned int DateTime::m_dpm = DPM; // days per month
unsigned int DateTime::m_mpy = MPY; // months per year

inline void DateTime::set(unsigned int t)
{
}

DateTime::DateTime(char * date_time) :
  m_second(0), m_minute(0), m_hour(0), m_day(0), m_month(0), m_year(0)
{
}

DateTime::DateTime(int t)
{
}


DateTime::DateTime(int yr, int mn, int da, int hr, int mt, int sc) :
  m_second(sc), m_minute(mt), m_hour(hr), m_day(da), m_month(mn), m_year(yr)
{
}

bool DateTime::isValid() const
{
    return false;
}

int DateTime::seconds()
{
    return 1;
}

void DateTime::update(int t)
{
}

std::string DateTime::asString()
{
    return "";
}

bool DateTime::operator==( const DateTime & date ) const
{
    return true;
}
