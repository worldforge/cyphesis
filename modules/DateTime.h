#ifndef DATE_TIME_H
#define DATE_TIME_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

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
