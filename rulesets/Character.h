// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_CHARACTER_H
#define RULESETS_CHARACTER_H

#include "Thing.h"

class Movement;
class BaseMind;

class Character : public Thing {
  protected:
    Movement & movement;
    bool autom;
    double drunkness;
    std::string sex;
    double food;
    double maxWeight;
    bool isAlive;

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
    BaseMind * externalMind;

    Character();
    virtual ~Character();

    virtual const Atlas::Message::Object& operator[](const std::string & aname);
    virtual void set(const std::string & aname, const Atlas::Message::Object & attr);

    virtual void addToObject(Atlas::Message::Object::MapType &) const;

    virtual oplist ImaginaryOperation(const Imaginary & op);
    virtual oplist SetupOperation(const Setup & op);
    virtual oplist TickOperation(const Tick & op);
    virtual oplist TalkOperation(const Talk & op);
    virtual oplist EatOperation(const Eat & op);
    virtual oplist NourishOperation(const Nourish & op);

    virtual oplist mindLoginOperation(const Login & op);
    virtual oplist mindCreateOperation(const Create & op);
    virtual oplist mindActionOperation(const Action & op);
    virtual oplist mindChopOperation(const Chop & op);
    virtual oplist mindCombineOperation(const Combine & op);
    virtual oplist mindCutOperation(const Cut & op);
    virtual oplist mindDeleteOperation(const Delete & op);
    virtual oplist mindDivideOperation(const Divide & op);
    virtual oplist mindEatOperation(const Eat & op);
    virtual oplist mindFireOperation(const Fire & op);
    virtual oplist mindGetOperation(const Get & op);
    virtual oplist mindImaginaryOperation(const Imaginary & op);
    virtual oplist mindInfoOperation(const Info & op);
    virtual oplist mindMoveOperation(const Move & op);
    virtual oplist mindNourishOperation(const Nourish & op);
    virtual oplist mindSetOperation(const Set & op);
    virtual oplist mindSightOperation(const Sight & op);
    virtual oplist mindSoundOperation(const Sound & op);
    virtual oplist mindTalkOperation(const Talk & op);
    virtual oplist mindTickOperation(const Tick & op);
    virtual oplist mindTouchOperation(const Touch & op);
    virtual oplist mindLookOperation(const Look & op);
    virtual oplist mindLoadOperation(const Load & op);
    virtual oplist mindSaveOperation(const Save & op);
    virtual oplist mindSetupOperation(const Setup & op);
    virtual oplist mindAppearanceOperation(const Appearance & op);
    virtual oplist mindDisappearanceOperation(const Disappearance & op);
    virtual oplist mindErrorOperation(const Error & op);
    virtual oplist mindOtherOperation(const RootOperation & op);

    virtual oplist w2mLoginOperation(const Login & op);
    virtual oplist w2mActionOperation(const Action & op);
    virtual oplist w2mChopOperation(const Chop & op);
    virtual oplist w2mCombineOperation(const Combine & op);
    virtual oplist w2mCreateOperation(const Create & op);
    virtual oplist w2mCutOperation(const Cut & op);
    virtual oplist w2mDeleteOperation(const Delete & op);
    virtual oplist w2mDivideOperation(const Divide & op);
    virtual oplist w2mEatOperation(const Eat & op);
    virtual oplist w2mFireOperation(const Fire & op);
    virtual oplist w2mGetOperation(const Get & op);
    virtual oplist w2mImaginaryOperation(const Imaginary & op);
    virtual oplist w2mInfoOperation(const Info & op);
    virtual oplist w2mMoveOperation(const Move & op);
    virtual oplist w2mNourishOperation(const Nourish & op);
    virtual oplist w2mSetOperation(const Set & op);
    virtual oplist w2mSightOperation(const Sight & op);
    virtual oplist w2mSoundOperation(const Sound & op);
    virtual oplist w2mTouchOperation(const Touch & op);
    virtual oplist w2mTickOperation(const Tick & op);
    virtual oplist w2mLookOperation(const Look & op);
    virtual oplist w2mLoadOperation(const Load & op);
    virtual oplist w2mSaveOperation(const Save & op);
    virtual oplist w2mSetupOperation(const Setup & op);
    virtual oplist w2mTalkOperation(const Talk & op);
    virtual oplist w2mAppearanceOperation(const Appearance & op);
    virtual oplist w2mDisappearanceOperation(const Disappearance & op);
    virtual oplist w2mErrorOperation(const Error & op);
    virtual oplist w2mOtherOperation(const RootOperation & op);

    virtual oplist sendMind(const RootOperation & op);
    virtual oplist mind2body(const RootOperation & op);
    virtual oplist world2body(const RootOperation & op);
    virtual oplist world2mind(const RootOperation & op);
    virtual oplist externalMessage(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist externalOperation(const RootOperation & op);
};

#endif // RULESETS_CHARACTER_H
