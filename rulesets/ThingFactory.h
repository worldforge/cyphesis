// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef THING_FACTORY_H
#define THING_FACTORY_H

class Thing;

class FactoryBase {
  public:
    virtual Thing * newThing() = 0;
};

template <typename T>
class ThingFactory : public FactoryBase {
  public:
    ThingFactory() { }
    Thing * newThing() { return new T(); }
};

#endif // THING_FACTORY_H
