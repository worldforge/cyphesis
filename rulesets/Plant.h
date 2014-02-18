// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef RULESETS_PLANT_H
#define RULESETS_PLANT_H

#include "Thing.h"

#include <boost/optional.hpp>

template <typename T>
class Property;

/// \brief This is the base class for flowering plants.
///
/// Most of the functionality will be common to all plants, and most
/// derived classes will probably be in python. Provides functionality
/// for producing and dropping fruit very simply.
///
/// The basic functionality of Plant is as follows:
/// 1) The plant receives a tick operation. It checks it's m_nourishment value to see
/// whether it should grow or wither.
/// If the plant has fruitName set, it will also do a check whether it should drop a fruit or not.
/// It also sends an Eat operation to it's parent.
/// 2) If the parent is the world, the world will respond to the Eat operation by checking
/// if the plant is in a favourable spot (i.e. a place where it can grow, like in the bare ground).
/// If so, the World will send a Nourishment op to the plant.
/// 3) The plant receives the Nourishment op and adds its value to m_nourishment.
///
/// \ingroup EntityClasses
class Plant : public Thing {
  protected:

    /**
     * Keeps track of nourishment received.
     *
     * Each tick this is checked. If it's zero or less, the plant will wither.
     * If it's positive the plant will grow.
     *
     * The value itself is increased through Nourishment ops.
     *
     * Note that we use an "optional" to prevent the first Tick operations to starve the plant
     * (before it has had time to nourish itself).
     */
    boost::optional<double> m_nourishment;

    static const int m_speed = 20; // Number of basic_ticks per tick
    static const int m_minuDrop = 0; // min fruit dropped
    static const int m_maxuDrop = 2; // max fruit dropped

    int dropFruit(OpVector & res, Property<int> * fruits_prop);
    /**
     * If there's an area attached to the plant it will be scaled according to the radius of the bounding box.
     */
    void scaleArea();

  public:

    explicit Plant(const std::string & id, long intId);
    virtual ~Plant();

    virtual void NourishOperation(const Operation &, OpVector &);
    virtual void TickOperation(const Operation &, OpVector &);
    virtual void TouchOperation(const Operation &, OpVector &);
};

#endif // RULESETS_PLANT_H
