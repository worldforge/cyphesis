// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef DATE_TIME_H
#define DATE_TIME_H

class DateTime {
    int second;
    int minute;
    int hour;
    int day;
    int month;
    int year;
  public:
    DateTime(char *);

    double seconds();
    char * asString();
};

#endif /* DATE_TIME_H */
