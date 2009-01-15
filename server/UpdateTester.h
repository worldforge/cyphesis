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

#ifndef SERVER_UPDATE_TESTER_H
#define SERVER_UPDATE_TESTER_H

#include "Idle.h"

/// \brief Spew type updates
///
/// A generic framework for running regular test events.
class UpdateTester : public Idle {
  public:
    explicit UpdateTester(CommServer & svr);
    virtual ~UpdateTester();

    virtual void idle(time_t t);
};

#endif // SERVER_UPDATE_TESTER_H
