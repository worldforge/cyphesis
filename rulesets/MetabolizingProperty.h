// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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


#ifndef RULESETS_METABOLIZING_PROPERTY_H
#define RULESETS_METABOLIZING_PROPERTY_H

#include "common/Property.h"

// MetabolizingProperty handles metabolism for living creatures
// Its value measures the amount of energy stored as mass 
// (This is suspect to change)
//
class MetabolizingProperty : public Property<double>
{
  private:

    /// \brief Handles the growth of the entity as an effect of mass increase
    void grow(LocatedEntity *, float scale);
    
    /// \brief Handles the increase of area around plant
    void scaleArea(LocatedEntity *);

  public:
 
    /// \brief How much energy we get burning one unit of mass
    static const double energyToMass;  

    /// \brief Energy unit for metabolism per tick
    // How much energy we consume (relative to our mass) to sustain ourselves per tick
    ///        Base value, modifed by metabolism speed.
    static const double energyUnitBase;  

    /// \brief Efficiency of digestion
    //static const double digsetEfficiency;

    /// \brief Relation how much energy creature is able to store as mass
    // relative to mass, one day should be made into atlas property
    static const double defaultReserves; 
 
    /// \brief How much mass creature is able to eat per tick (relative to its mass)
    ///        Base value, modifed by metabolism speed.
    static const double biteSizeBase;

    virtual void install(LocatedEntity *, const std::string &);
    virtual void remove(LocatedEntity *, const std::string &);
    virtual HandlerResult operation(LocatedEntity *,
                                    const Operation &,
                                    OpVector &);
    virtual MetabolizingProperty * copy() const;

    HandlerResult tick_handler(LocatedEntity * e,
                              const Operation & op,
                              OpVector & res);
};

#endif // RULESETS_METABOLIZING_PROPERTY_H
