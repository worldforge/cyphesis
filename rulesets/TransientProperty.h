// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#ifndef RULESETS_TRANSIENT_PROPERTY_H
#define RULESETS_TRANSIENT_PROPERTY_H

#include "common/Property.h"

/// \brief Class to handle Entity which are ephemeral and get auto deleted
/// \ingroup PropertyClasses
class TransientProperty : public Property<double> {
  public:
    TransientProperty();
    virtual ~TransientProperty();

    virtual TransientProperty * copy() const;

    virtual void install(LocatedEntity *, const std::string &);

    virtual void apply(LocatedEntity *);
};

#endif // RULESETS_TRANSIENT_PROPERTY_H
