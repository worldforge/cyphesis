// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef RULESETS_STACKABLE_H
#define RULESETS_STACKABLE_H

#include "Thing.h"

typedef Thing Stackable_parent;

/// \brief This is the base class for items which are stackable objects, which
/// are mostly used in multiple quantities, if there is usually no difference.
/// between individual items.
///
/// A good example is coins which are unmanageable if each one must be
/// represented by an object. This class is not yet implemented as Combine
/// and Divide semantics are not yet sorted. It may well be obsoleted if
/// piles are implemented as was suggested in some early Mason docs.
class Stackable : public Stackable_parent {
  protected:
    int m_num; // Number of items

  public:

    explicit Stackable(const std::string & id);
    virtual ~Stackable();

    virtual bool get(const std::string &, Element &) const;
    virtual void set(const std::string &, const Element &);

    virtual void addToMessage(MapType &) const;

    virtual void CombineOperation(const Combine &, OpVector &);
    virtual void DivideOperation(const Divide &, OpVector &);
};

#endif // RULESETS_STACKABLE_H
