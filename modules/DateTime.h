// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_DATE_TIME_H
#define COMMON_DATE_TIME_H

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

    void set(int);
  public:
    explicit DateTime(char *);
    explicit DateTime(int);
    DateTime(int, int, int, int, int, int);

    //bool operator==(const DateTime & other) const { }

    static void define(unsigned int spm, unsigned int mph, unsigned int hpd,
                       unsigned int dpm, unsigned int mpy) {
        m_spm = spm; m_mph = mph; m_hpd = hpd; m_dpm = dpm; m_mpy = mpy;
    }

    int seconds();
    void update(int);
    std::string asString();

    int second() const { return m_second; }
    int minute() const { return m_minute; }
    int hour() const { return m_hour; }
    int day() const { return m_day; }
    int month() const { return m_month; }
    int year() const { return m_year; }

    unsigned int spm() { return m_spm; }
    unsigned int mph() { return m_mph; }
    unsigned int hpd() { return m_hpd; }
    unsigned int dpm() { return m_dpm; }
    unsigned int mpy() { return m_mpy; }
};

inline std::ostream & operator<<(std::ostream& s, const DateTime& d) {
    return s << d.year() << "-" << d.month() << "-" << d.day() << " "
             << d.hour() << ":" << d.minute() << ":" << d.second();
}


#endif // COMMON_DATE_TIME_H
