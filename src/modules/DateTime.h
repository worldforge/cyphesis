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


#ifndef MODULES_DATE_TIME_H
#define MODULES_DATE_TIME_H

#include <string>
#include <iostream>

class DateTime {
  protected:
    int m_second;
    int m_minute;
    int m_hour;
    int m_day;
    int m_month;
    int m_year;

    static int m_spm; // seconds per minute
    static int m_mph; // minutes per hour
    static int m_hpd; // hours per day
    static int m_dpm; // days per month
    static int m_mpy; // months per year

    void set(int);
  public:
    explicit DateTime(char *);
    explicit DateTime(int);
    DateTime(int, int, int, int, int, int);

    bool isValid() const;

    static void define(int spm, int mph, int hpd,
                       int dpm, int mpy) {
        m_spm = spm; m_mph = mph; m_hpd = hpd; m_dpm = dpm; m_mpy = mpy;
    }

    int seconds() const;
    void update(int);
    std::string asString();

    int second() const { return m_second; }
    int minute() const { return m_minute; }
    int hour() const { return m_hour; }
    int day() const { return m_day; }
    int month() const { return m_month; }
    int year() const { return m_year; }

    static int spm() { return m_spm; }
    static int mph() { return m_mph; }
    static int hpd() { return m_hpd; }
    static int dpm() { return m_dpm; }
    static int mpy() { return m_mpy; }

    bool operator==( const DateTime & ) const;
};

inline std::ostream & operator<<(std::ostream& s, const DateTime& d) {
    return s << d.year() << "-" << d.month() << "-" << d.day() << " "
             << d.hour() << ":" << d.minute() << ":" << d.second();
}


#endif // MODULES_DATE_TIME_H
