// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_PLANT_H
#define RULESETS_PLANT_H

#include "Thing.h"

// This is the base class for flowering plants. Most of the functionality
// will be common to all plants, and most derived classes will probably
// be in python.

class Plant : public Thing {
  protected:
    int fruits; // Number of fruits on the plant
    int radius; // Proportion of height as radius
    int fruitChance; // chance of growing fruit
    double sizeAdult; // chance of growing fruit
    std::string fruitName;

    static const int speed = 20; // Number of basic_ticks per tick
    static const int minuDrop = 0; // min fruit dropped
    static const int maxuDrop = 2; // max fruit dropped

    int dropFruit(OpVector & res);
  public:

    explicit Plant(const std::string & id);
    virtual ~Plant();

    virtual bool get(const std::string &, Atlas::Message::Object &) const;
    virtual void set(const std::string &, const Atlas::Message::Object &);

    virtual OpVector TickOperation(const Tick & op);
};

#endif // RULESETS_PLANT_H
