// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_PLANT_H
#define RULESETS_PLANT_H

#include "Thing.h"

// This is the base class for flowering plants. Most of the functionality
// will be common to all plants, and most derived classes will probably
// be in python.

typedef Thing Plant_parent;

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

    virtual bool get(const std::string &, Atlas::Message::Object &) const;
    virtual void set(const std::string &, const Atlas::Message::Object &);
    virtual void addToObject(Atlas::Message::Object::MapType & obj) const;

    virtual OpVector TickOperation(const Tick & op);
};

#endif // RULESETS_PLANT_H
