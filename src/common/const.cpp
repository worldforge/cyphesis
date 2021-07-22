// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#include "Version.h"
#include "const.h"

namespace consts {

  // Id of root world entity
  const char * rootWorldId = "0";
  // Integer id of root world entity
  const long rootWorldIntId = 0L;
  // Version of the software we are running
  const char * version = VERSION;
  // Bump this whenever there's a client breaking change to the protocol. Use the current date (ISO format).
  const long protocol_version = 20210722;
}
