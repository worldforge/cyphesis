#ifndef WORLD_TIME_H
#define WORLD_TIME_H

#include <modules/DateTime.h>

// timedata time2type(const string & t);

class WorldTime {
    //DateTime time;
    //string/int month;
    double secs;

  public:
    WorldTime(double seconds) : secs(seconds) { }
    double seconds() { return secs; }
    //WorldTime(char * date_time);
    //string & __getattr__(const string & name);
    //string & __repr__();
    //string & __str__();
    //bool operator==(const WorldTime & other);
};

const string & seconds2string(double seconds);

#endif /* WORLD_TIME_H */
