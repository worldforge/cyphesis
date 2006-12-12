// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Stackable.h,v 1.28 2006-12-12 15:54:24 alriddoch Exp $

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
/// \ingroup EntityClasses
class Stackable : public Stackable_parent {
  protected:
    /// \brief Number of items in the stack
    int m_num;

  public:

    explicit Stackable(const std::string & id, long intId);
    virtual ~Stackable();

    virtual void CombineOperation(const Operation &, OpVector &);
    virtual void DivideOperation(const Operation &, OpVector &);
};

#endif // RULESETS_STACKABLE_H
