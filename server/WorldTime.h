#ifndef WORLD_TIME_H
#define WORLD_TIME_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

#include <modules/DateTime.h>

bad_type time2type(bad_type t);

class WorldTime {
    //DateTime time;
    //bad_type month;
    //bad_type __dict__;
    double secs;

  public:
    WorldTime(double seconds) : secs(seconds) { }
    double seconds() { return secs; }
    //WorldTime(char * date_time);
    //bad_type __getattr__(bad_type name);
    //bad_type seconds();
    //bad_type __repr__();
    //bad_type __str__();
    //bool operator==(const WorldTime & other);
    //bad_type __call__();
};

bad_type seconds2string(bad_type seconds);

#endif /* WORLD_TIME_H */
