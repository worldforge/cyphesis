// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_THING_FACTORY_H
#define RULESETS_THING_FACTORY_H

#include <Atlas/Message/Object.h>

class Thing;

class FactoryBase {
  public:
    std::string script;
    std::string language;
    Atlas::Message::Object::MapType attributes;

    virtual ~FactoryBase() { }

    virtual Thing * newThing() = 0;
    virtual FactoryBase * dupFactory() = 0;
};

template <typename T>
class ThingFactory : public FactoryBase {
  public:
    ThingFactory() { }
    Thing * newThing() { return new T(); }
    FactoryBase * dupFactory() { return new ThingFactory<T>(); }
};

#endif // RULESETS_THING_FACTORY_H
