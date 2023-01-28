/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CYPHESIS_USAGEINSTANCE_H
#define CYPHESIS_USAGEINSTANCE_H

#include "rules/entityfilter/Filter.h"
#include "rules/EntityLocation.h"
#include "modules/Ref.h"
#include "common/Property.h"

#include <Atlas/Objects/RootOperation.h>

#include <boost/variant.hpp>

#include <memory>
#include <functional>

namespace Py
{
class Object;
}

/**
 * This struct defines the requirements for a parameter.
 */
struct UsageParameter {
    typedef boost::variant<EntityLocation, WFMath::Point<3>, WFMath::Vector<3>> UsageArg;

    /**
     * The type of parameter.
     */
    enum class Type {
        /**
         * An entity reference is required.
         */
        ENTITY,
        /**
         * An entity location is required. This means that there must both be an entity as well as a position.
         */
        ENTITYLOCATION,
        /**
         * A position (i.e. a WFMath::Point<3>) is required.
         */
        POSITION,
        /**
         * A direction (i.e. a WFMath::Vector<3>) is required.
         */
        DIRECTION
    };

    /**
     * The type of this parameter.
     */
    Type type;
    /**
     * An optional constraint.
     * A shared_ptr to allow for easier Python bindings.
     */
    std::shared_ptr<EntityFilter::Filter> constraint;
    /*
     * The minimum number of entries required for this parameter.
     * Defaults to 1.
     */
    int min = 1;
    /*
     * The maximum number of entries required for this parameter.
     * Defaults to 1.
     */
    int max = 1;

    static UsageParameter parse(const Atlas::Message::Element& element);

    int countValidArgs(const std::vector<UsageArg>& args, const Ref<LocatedEntity>& actor, const Ref<LocatedEntity>& tool, std::vector<std::string>& errorMessages) const;
};

struct Usage {
    std::string description;

    std::map<std::string, UsageParameter> params;

    /**
     * The Python script which will handle this op.
     */
    std::string handler;
    std::shared_ptr<EntityFilter::Filter> constraint;
};

class UsageInstance
{
  public:
    static std::function<Py::Object(UsageInstance&& usageInstance)> scriptCreator;

    /**
     * The usage definition.
     */
    Usage definition;

    /**
     * The entity performing the usage.
     */
    Ref<LocatedEntity> actor;

    /**
     * The entity containing the usage.
     */
    Ref<LocatedEntity> tool;

    /**
     * Any arguments sent along.
     */
    std::map<std::string, std::vector<UsageParameter::UsageArg>> args;

    /**
     * The operation triggering the usage.
     */
    Atlas::Objects::Operation::RootOperation op;

    std::pair<bool, std::string> isValid() const;
};

#endif  // CYPHESIS_USAGEINSTANCE_H
