// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include <Python.h>

#include <varconf/Config.h>

#include <string.h>

#include <common/BaseEntity.h>

#include "Python_API.h"

extern varconf::Config * global_conf;

class ThingFactory {
  public:
    ThingFactory() { }
    void readRuleset(const string & file);
    Thing * newThing(const string &, const Message::Object &, Routing *);
};

extern ThingFactory thing_factory;

#endif /* THING_FACTORY_H */
