import string,re
from types import *

date_pat=re.compile("^|[-:]|\s+")

class DateTime:
    def __init__(self, date_time):
        if type(date_time)==StringType:
            (self.year,self.month,self.day,
             self.hour,self.minute,self.second)=\
             map(float,date_pat.split(date_time))
            #print "DateTime(string:",date_time,")=",self
        else:
            tmp=date_time
            date_time,self.second=divmod(date_time,60)
            date_time,self.minute=divmod(date_time,60)
            date_time,self.hour=divmod(date_time,24)
            date_time,self.day=divmod(date_time,30)
            date_time,self.month=divmod(date_time,12)
            self.year=date_time
            self.month=self.month+1
            self.day=self.day+1
            #print "DateTime(number:",tmp,")=",self
    def seconds(self):
        return self.second+\
               self.minute*60.0+\
               self.hour*3600+\
               (self.day-1)*86400.0+\
               (self.month-1)*2592000.0+\
               self.year*31104000.0
    def __str__(self):
        return "%04i-%02i-%02i %02i:%02i:%04.1f" % \
               (self.year,self.month,self.day,
                self.hour,self.minute,self.second)


