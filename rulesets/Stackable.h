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

typedef Thing Stackable_parent;

class Stackable : public Stackable_parent {
  protected:
    int m_num; // Number of items

  public:

    explicit Stackable(const std::string & id);
    virtual ~Stackable();

    virtual bool get(const std::string &, Element &) const;
    virtual void set(const std::string &, const Element &);

    virtual void addToObject(MapType &) const;

    virtual OpVector CombineOperation(const Combine & op);
    virtual OpVector DivideOperation(const Divide & op);
};

#endif // RULESETS_STACKABLE_H
