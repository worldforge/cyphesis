// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef CHARACTER_H
#define CHARACTER_H

class BaseMind;
class ExternalMind;
class Account;
class Character;
class Location;
class Movement;

#include "Thing.h"

#include <physics/Vector3D.h>

class Movement;

class Character : public Thing {
  protected:
    Movement & movement;
    int autom;

    static const double energyConsumption = 0.001;
    static const double foodConsumption = 0.1;
    static const double weightConsumption = 1.0;
    static const double energyGain = 0.5;
    static const double energyLoss = 0.1;
    static const double weightGain = 0.5;

    oplist metabolise(double ammount = 1); 

    friend class Movement;
  public:
    BaseMind * mind;
    ExternalMind * externalMind;
    double drunkness;
    string sex;
    double food;
    double maxWeight;

    Character();
    virtual ~Character();

    virtual const Atlas::Message::Object & operator[](const string & aname);
    virtual void set(const string & aname, const Atlas::Message::Object & attr);

    virtual void addToObject(Atlas::Message::Object *) const;
    virtual oplist Operation(const Setup & op);
    virtual oplist Operation(const Tick & op);
    virtual oplist Operation(const Talk & op);
    virtual oplist Operation(const Eat & op);
    virtual oplist Operation(const Nourish & op);
    virtual oplist mindOperation(const Login & op);
    virtual oplist mindOperation(const Create & op);
    virtual oplist mindOperation(const Cut & op);
    virtual oplist mindOperation(const Delete & op);
    virtual oplist mindOperation(const Eat & op);
    virtual oplist mindOperation(const Move & op);
    virtual oplist mindOperation(const Set & op);
    virtual oplist mindOperation(const Sight & op);
    virtual oplist mindOperation(const Sound & op);
    virtual oplist mindOperation(const Talk & op);
    virtual oplist mindOperation(const Tick & op);
    virtual oplist mindOperation(const Touch & op);
    virtual oplist mindOperation(const Look & op);
    virtual oplist mindOperation(const Load & op);
    virtual oplist mindOperation(const Save & op);
    virtual oplist mindOperation(const Setup & op);
    virtual oplist mindOperation(const Appearance & op);
    virtual oplist mindOperation(const Disappearance & op);
    virtual oplist mindOperation(const Error & op);
    virtual oplist mindOperation(const RootOperation & op);
    virtual oplist w2mOperation(const Login & op);
    virtual oplist w2mOperation(const Chop & op);
    virtual oplist w2mOperation(const Create & op);
    virtual oplist w2mOperation(const Cut & op);
    virtual oplist w2mOperation(const Delete & op);
    virtual oplist w2mOperation(const Eat & op);
    virtual oplist w2mOperation(const Fire & op);
    virtual oplist w2mOperation(const Move & op);
    virtual oplist w2mOperation(const Set & op);
    virtual oplist w2mOperation(const Sight & op);
    virtual oplist w2mOperation(const Sound & op);
    virtual oplist w2mOperation(const Touch & op);
    virtual oplist w2mOperation(const Tick & op);
    virtual oplist w2mOperation(const Look & op);
    virtual oplist w2mOperation(const Load & op);
    virtual oplist w2mOperation(const Save & op);
    virtual oplist w2mOperation(const Setup & op);
    virtual oplist w2mOperation(const Appearance & op);
    virtual oplist w2mOperation(const Disappearance & op);
    virtual oplist w2mOperation(const Error & op);
    virtual oplist w2mOperation(const RootOperation & op);
    virtual oplist sendMind(const RootOperation & op);
    virtual oplist mind2body(const RootOperation & op);
    virtual oplist world2body(const RootOperation & op);
    virtual oplist world2mind(const RootOperation & op);
    virtual oplist externalMessage(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist externalOperation(const RootOperation & op);
};

#endif /* CHARACTER_H */
