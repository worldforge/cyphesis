// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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


#ifndef COMMON_SYSTEM_TIME_H
#define COMMON_SYSTEM_TIME_H

#include <sys/time.h>

class SystemTime {
  protected:
    struct timeval m_timeVal;
  public:
    time_t seconds() const {
        return m_timeVal.tv_sec;
    }

    time_t microseconds() const {
        return m_timeVal.tv_usec;
    }
    void update() {
        gettimeofday(&m_timeVal, 0);
    }
};

#endif // COMMON_SYSTEM_TIME_H
