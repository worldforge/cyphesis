#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from DateTime import *
from common import const

#rule: time->importance       : spring -> seasonal activity
#mind: time2num[a.time]==time : spring -> 3,4,5
#      importance comparison  : 

time_info={'always': [range(1,13),'seasonal'],
           'spring': [[3,4,5],'seasonal'],
           'summer': [[6,7,8],'seasonal'],
           'autumn': [[9,10,11],'seasonal'],
           'winter': [[12,1,2],'seasonal'],
           'morning': [[8], 'daily'],
           'midday': [[12], 'daily'],
           'evening': [[20], 'daily'],
           'night': [[21,22,23,24,0,1,2,3,4,5,6,7], 'daily'],
           'now': [range(25), 'daily']}
#23-24,1-4: night
#5-10: morning
#11-16: day
#17-22: evening

month2season={}
for season in ['spring','summer','autumn','winter']:
    for month in time_info[season][0]:
        month2season[month]=season

def time2type(t):
    return time_info[t][1]

time2importance=time2type

## importance2num={None: 0,
##                 'seasonal': 1,
##                 'daily': 2}

## def cmp_importance(a,b):
##     return importance2num[a]-importance2num[b]

def minutes(n): return n*60.0
def hours(n): return minutes(60)*n
def days(n): return hours(24)*n
def months(n): return days(30)*n
def years(n): return months(12)*n

#time functions for world
class WorldTime:
    def __init__(self, date_time="1-1-1 0:0:0"):
        self.time=DateTime(date_time)
    def __getattr__(self, name):
        t=self.__dict__['time']
        if t.__dict__.has_key(name):
            return t.__dict__[name]
        if name=="season":
            return month2season[self.month]
        raise AttributeError, name
    def __call__(self):
        return self.time
    def __repr__(self):
        return `self.time.seconds()`
    def __str__(self):
        return str(self.time)
    def __cmp__(self,other):
        if time2type(other)=='seasonal':
            if self.time.month in time_info[other][0]:
                return 0
            else:
                return 1
        if time2type(other)=='daily':
            if self.time.hour in time_info[other][0]:
                return 0
            else:
                return 1
        return 1
    def seconds(self):
        return self.time.seconds()

def seconds2string(seconds):
    date_time=DateTime(seconds)
    return str(date_time)


