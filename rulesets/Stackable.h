// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef STACKABLE_H
#define STACKABLE_H

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

    virtual const Atlas::Message::Object & operator[](const string & aname);
    virtual void set(const string & aname, const Atlas::Message::Object & attr);

    virtual void addToObject(Atlas::Message::Object &) const;

    virtual oplist CombineOperation(const Combine & op);
    virtual oplist DivideOperation(const Divide & op);
};

#endif // STACKABLE_H
