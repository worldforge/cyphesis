// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
class Plant : public Plant_parent {
  protected:
    int m_fruits; // Number of fruits on the plant
    int m_radius; // Proportion of height as radius
    int m_fruitChance; // chance of growing fruit
    double m_sizeAdult; // chance of growing fruit
    std::string m_fruitName;

    static const int m_speed = 20; // Number of basic_ticks per tick
    static const int m_minuDrop = 0; // min fruit dropped
    static const int m_maxuDrop = 2; // max fruit dropped

    int dropFruit(OpVector & res);
  public:

    explicit Plant(const std::string & id);
    virtual ~Plant();

    const int getFruits() const { return m_fruits; }
    const std::string & getFruitName() const { return m_fruitName; }
    const int getFruitChance() const { return m_fruitChance; }
    const double getSizeAdult() const { return m_sizeAdult; }

    virtual bool get(const std::string &, Element &) const;
    virtual void set(const std::string &, const Element &);
    virtual void addToMessage(MapType & obj) const;

    virtual void TickOperation(const Tick &, OpVector &);
    virtual void TouchOperation(const Touch &, OpVector &);
};

#endif // RULESETS_PLANT_H
