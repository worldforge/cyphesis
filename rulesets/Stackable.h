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

    explicit Stackable(const std::string & id);
    virtual ~Stackable();

    virtual bool get(const std::string &, Atlas::Message::Object &) const;
    virtual void set(const std::string &, const Atlas::Message::Object &);

    virtual void addToObject(Atlas::Message::Object::MapType &) const;

    virtual OpVector CombineOperation(const Combine & op);
    virtual OpVector DivideOperation(const Divide & op);
};

#endif // RULESETS_STACKABLE_H
