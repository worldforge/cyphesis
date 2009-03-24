// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

// $Id: 04fe51e66634b1f3b6e850d4e1925d9d1d2f5a3f $

#ifndef SERVER_MASTER_H
#define SERVER_MASTER_H

#include "common/Identified.h"

class CommClient;
class ServerRouting;

/// \brief Class representing connections from a master server to which this
/// program is a slave.
///
/// This is designed to be used from a cyphesis process running as an AI slave.
class Master : public Router {
  protected:
    
  public:
    CommClient & m_commClient;
    ServerRouting & m_server;

    Master(CommClient & client, ServerRouting & svr, const std::string & id);
    virtual ~Master();

    virtual void operation(const Operation &, OpVector &);
};

#endif // SERVER_MASTER_H
