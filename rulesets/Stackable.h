// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_STACKABLE_H
#define RULESETS_STACKABLE_H

#include "Thing.h"

// This is the base class for items which are stackable objects, which are
// mostly used in multiple quantities, and there is usually no difference
// between individual items. A good example is coins which are unmanageable
// if each one must be represented by an object.


class Stackable : public Thing {
  protected:
    int num; // Number of fruits on the plant

  public:

    Stackable();
    virtual ~Stackable();

    virtual const Atlas::Message::Object get(const std::string &) const;
    virtual void set(const std::string & aname, const Atlas::Message::Object & attr);

    virtual void addToObject(Atlas::Message::Object::MapType &) const;

    virtual OpVector CombineOperation(const Combine & op);
    virtual OpVector DivideOperation(const Divide & op);
};

#endif // RULESETS_STACKABLE_H
