// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include "DateTime.h"

// date_pat=re.compile("^|[-:]|\s+");

DateTime::DateTime(char * date_time)
{
	// extract numbers from string
        //(DateTime::year,DateTime::month,DateTime::day,;
         //DateTime::hour,DateTime::minute,DateTime::second)=\;
         //map(float,date_pat.split(date_time));
}

double DateTime::seconds()
{
    return DateTime::second+
           minute*60.0+
           hour*3600.0+
           (day-1)*86400.0+
           (month-1)*2592000.0+
           year*31104000.0;
}

char * DateTime::asString()
{
    //Convert date into string
    //return "%04i-%02i-%02i %02i:%02i:%04.1f" %
         //(DateTime::year,DateTime::month,DateTime::day,;
            //DateTime::hour,DateTime::minute,DateTime::second);
}
