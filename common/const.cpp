// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "const.h"

namespace consts {

  // FIXME
  // Admin password used by admin account
  const char * defaultAdminPasswordHash = "$1$1A67C3C65EECBBCE$A51127573498DCB08992378D07A36A20";
  // Id of root world entity
  // const char * rootWorldId = "world_0";
  const char * rootWorldId = "0";
  // Version of the software we are running
  const char * version = VERSION;
  // Time this server was built
  const char * buildTime = __TIME__;
  const char * buildDate = __DATE__;

}
