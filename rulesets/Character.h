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

    OpVector metabolise(double ammount = 1); 

    friend class Movement;
  public:
    BaseMind * mind;
    BaseMind * externalMind;

    Character();
    virtual ~Character();

    virtual const Atlas::Message::Object& operator[](const std::string & aname);
    virtual void set(const std::string & aname, const Atlas::Message::Object & attr);

    virtual void addToObject(Atlas::Message::Object::MapType &) const;

    virtual OpVector operation(const RootOperation & op);
    virtual OpVector externalMessage(const RootOperation & op);
    virtual OpVector externalOperation(const RootOperation & op);

    virtual OpVector ImaginaryOperation(const Imaginary & op);
    virtual OpVector SetupOperation(const Setup & op);
    virtual OpVector TickOperation(const Tick & op);
    virtual OpVector TalkOperation(const Talk & op);
    virtual OpVector EatOperation(const Eat & op);
    virtual OpVector NourishOperation(const Nourish & op);

    virtual OpVector mindLoginOperation(const Login & op);
    virtual OpVector mindCreateOperation(const Create & op);
    virtual OpVector mindActionOperation(const Action & op);
    virtual OpVector mindChopOperation(const Chop & op);
    virtual OpVector mindCombineOperation(const Combine & op);
    virtual OpVector mindCutOperation(const Cut & op);
    virtual OpVector mindDeleteOperation(const Delete & op);
    virtual OpVector mindDivideOperation(const Divide & op);
    virtual OpVector mindEatOperation(const Eat & op);
    virtual OpVector mindFireOperation(const Fire & op);
    virtual OpVector mindGetOperation(const Get & op);
    virtual OpVector mindImaginaryOperation(const Imaginary & op);
    virtual OpVector mindInfoOperation(const Info & op);
    virtual OpVector mindMoveOperation(const Move & op);
    virtual OpVector mindNourishOperation(const Nourish & op);
    virtual OpVector mindSetOperation(const Set & op);
    virtual OpVector mindSightOperation(const Sight & op);
    virtual OpVector mindSoundOperation(const Sound & op);
    virtual OpVector mindTalkOperation(const Talk & op);
    virtual OpVector mindTickOperation(const Tick & op);
    virtual OpVector mindTouchOperation(const Touch & op);
    virtual OpVector mindLookOperation(const Look & op);
    virtual OpVector mindLoadOperation(const Load & op);
    virtual OpVector mindSaveOperation(const Save & op);
    virtual OpVector mindSetupOperation(const Setup & op);
    virtual OpVector mindAppearanceOperation(const Appearance & op);
    virtual OpVector mindDisappearanceOperation(const Disappearance & op);
    virtual OpVector mindErrorOperation(const Error & op);
    virtual OpVector mindOtherOperation(const RootOperation & op);

    virtual OpVector w2mLoginOperation(const Login & op);
    virtual OpVector w2mActionOperation(const Action & op);
    virtual OpVector w2mChopOperation(const Chop & op);
    virtual OpVector w2mCombineOperation(const Combine & op);
    virtual OpVector w2mCreateOperation(const Create & op);
    virtual OpVector w2mCutOperation(const Cut & op);
    virtual OpVector w2mDeleteOperation(const Delete & op);
    virtual OpVector w2mDivideOperation(const Divide & op);
    virtual OpVector w2mEatOperation(const Eat & op);
    virtual OpVector w2mFireOperation(const Fire & op);
    virtual OpVector w2mGetOperation(const Get & op);
    virtual OpVector w2mImaginaryOperation(const Imaginary & op);
    virtual OpVector w2mInfoOperation(const Info & op);
    virtual OpVector w2mMoveOperation(const Move & op);
    virtual OpVector w2mNourishOperation(const Nourish & op);
    virtual OpVector w2mSetOperation(const Set & op);
    virtual OpVector w2mSightOperation(const Sight & op);
    virtual OpVector w2mSoundOperation(const Sound & op);
    virtual OpVector w2mTouchOperation(const Touch & op);
    virtual OpVector w2mTickOperation(const Tick & op);
    virtual OpVector w2mLookOperation(const Look & op);
    virtual OpVector w2mLoadOperation(const Load & op);
    virtual OpVector w2mSaveOperation(const Save & op);
    virtual OpVector w2mSetupOperation(const Setup & op);
    virtual OpVector w2mTalkOperation(const Talk & op);
    virtual OpVector w2mAppearanceOperation(const Appearance & op);
    virtual OpVector w2mDisappearanceOperation(const Disappearance & op);
    virtual OpVector w2mErrorOperation(const Error & op);
    virtual OpVector w2mOtherOperation(const RootOperation & op);

    virtual OpVector sendMind(const RootOperation & op);
    virtual OpVector mind2body(const RootOperation & op);
    virtual OpVector world2body(const RootOperation & op);
    virtual OpVector world2mind(const RootOperation & op);
};

#endif // RULESETS_CHARACTER_H
