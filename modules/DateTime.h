// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <string>

class DateTime {
  protected:
    int m_second;
    int m_minute;
    int m_hour;
    int m_day;
    int m_month;
    int m_year;

    static unsigned int m_spm; // seconds per minute
    static unsigned int m_mph; // minutes per hour
    static unsigned int m_hpd; // hours per day
    static unsigned int m_dpm; // days per month
    static unsigned int m_mpy; // months per year
  public:
    DateTime(char *);
    DateTime(int, int, int, int, int, int);

    static void define(unsigned int spm, unsigned int mph, unsigned int hpd,
                       unsigned int dpm, unsigned int mpy) {
        m_spm = spm; m_mph = mph; m_hpd = hpd; m_dpm = dpm; m_mpy = mpy;
    }

    double seconds();
    std::string asString();

    int second() { return m_second; }
    int minute() { return m_minute; }
    int hour() { return m_hour; }
    int day() { return m_day; }
    int month() { return m_month; }
    int year() { return m_year; }

    unsigned int spm() { return m_spm; }
    unsigned int mph() { return m_mph; }
    unsigned int hpd() { return m_hpd; }
    unsigned int dpm() { return m_dpm; }
    unsigned int mpy() { return m_mpy; }
};

#endif /* DATE_TIME_H */
