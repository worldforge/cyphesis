// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#ifndef TOOLS_OPERATION_MONITOR_H
#define TOOLS_OPERATION_MONITOR_H

#include "AdminTask.h"

/// \brief Task class for monitoring all in-game operations occuring.
class OperationMonitor : public AdminTask {
  protected:
    int op_count;
    int start_time;
  public:
    int count() {
        return op_count;
    }

    int startTime() {
        return start_time;
    }

    virtual void setup(const std::string & arg, OpVector &);
    virtual void operation(const Operation & op, OpVector &);
};

#endif // TOOLS_OPERATION_MONITOR_H
