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

// $Id$

#ifndef RULESETS_PLANT_H
#define RULESETS_PLANT_H

#include "Thing.h"

typedef Thing Plant_parent;

/// \brief This is the base class for flowering plants.
///
/// Most of the functionality will be common to all plants, and most
/// derived classes will probably be in python. Provides functionality
/// for producing and dropping fruit very simply, currently accelerated
/// for Acorn compatability. In the longer term this should provide for
/// plants to grow, assuming we need plants to grow now that we are
/// using Mercator for forests.
/// \ingroup EntityClasses
class Plant : public Plant_parent {
  protected:
    int m_fruits; // Number of fruits on the plant
    int m_fruitChance; // chance of growing fruit
    double m_sizeAdult; // chance of growing fruit

    double m_nourishment;

    static const int m_speed = 20; // Number of basic_ticks per tick
    static const int m_minuDrop = 0; // min fruit dropped
    static const int m_maxuDrop = 2; // max fruit dropped

    int dropFruit(OpVector & res);
  public:

    explicit Plant(const std::string & id, long intId);
    virtual ~Plant();

    const int getFruits() const { return m_fruits; }
    const int getFruitChance() const { return m_fruitChance; }
    const double getSizeAdult() const { return m_sizeAdult; }

    virtual void NourishOperation(const Operation &, OpVector &);
    virtual void SetupOperation(const Operation &, OpVector &);
    virtual void TickOperation(const Operation &, OpVector &);
    virtual void TouchOperation(const Operation &, OpVector &);
};

#endif // RULESETS_PLANT_H
