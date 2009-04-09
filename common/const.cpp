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

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "const.h"

namespace consts {

  // Admin password used by admin account
  const char * defaultAdminPasswordHash = "$1$1A67C3C65EECBBCE$A51127573498DCB08992378D07A36A20";
  // Id of root world entity
  const char * rootWorldId = "0";
  // Integer id of root world entity
  const long rootWorldIntId = 0L;
  // Version of the software we are running
  const char * version = VERSION;
  // Time this server was built
  // const char * buildTime = __TIME__;
  // const char * buildDate = __DATE__;

}
