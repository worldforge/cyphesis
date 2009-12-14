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

#ifndef TOOLS_WORLD_DUMPER_H
#define TOOLS_WORLD_DUMPER_H

#include "common/ClientTask.h"

#include <list>

/// \brief Task class for dumping the world to a file
class WorldDumper : public ClientTask {
  protected:
    std::string m_account;
    int m_lastSerialNo;
    std::list<std::string> m_queue;
    int m_count;

    void infoArrived(const Operation &, OpVector & res);
  public:
    explicit WorldDumper(const std::string & accountId);

    virtual void setup(const std::string & arg, OpVector & ret);
    virtual void operation(const Operation & op, OpVector & res);
};

#endif // TOOLS_WORLD_DUMPER_H
