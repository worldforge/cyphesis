// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef STACKABLE_H
#define STACKABLE_H

#include "Thing.h"

// This is the base class for flowering plants. Most of the functionality
// will be common to all plants, and most derived classes will probably
// be in python.


class Stackable : public Thing {
  protected:
    int num; // Number of fruits on the plant

  public:

    Stackable();
    virtual ~Stackable() { }

    virtual const Object & operator[](const string & aname);
    virtual void set(const string & aname, const Object & attr);

    virtual void addObject(Message::Object *) const;
    virtual oplist Operation(const Combine & op);
    virtual oplist Operation(const Divide & op);
};

#endif // STACKABLE_H
