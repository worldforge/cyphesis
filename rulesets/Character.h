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
    double drunkness;
    std::string sex;
    double food;
    double maxMass;
    bool isAlive;

    static const double energyConsumption = 0.001;
    static const double foodConsumption = 0.1;
    static const double weightConsumption = 1.0;
    static const double energyGain = 0.5;
    static const double energyLoss = 0.1;
    static const double weightGain = 0.5;

    OpVector metabolise(double ammount = 1); 

    friend class Movement;
  protected:
    OpNoDict opMindLookup;
    OpNoDict opW2mLookup;

    void mindSubscribe(const std::string& op, OpNo no) {
        opMindLookup[op] = no;
    }

    void w2mSubscribe(const std::string& op, OpNo no) {
        opW2mLookup[op] = no;
    }
  public:
    BaseMind * mind;
    BaseMind * externalMind;

    Character();
    virtual ~Character();

    virtual const Atlas::Message::Object get(const std::string &) const;
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

    OpVector mindLoginOperation(const Login & op);
    OpVector mindCreateOperation(const Create & op);
    OpVector mindActionOperation(const Action & op);
    OpVector mindChopOperation(const Chop & op);
    OpVector mindCombineOperation(const Combine & op);
    OpVector mindCutOperation(const Cut & op);
    OpVector mindDeleteOperation(const Delete & op);
    OpVector mindDivideOperation(const Divide & op);
    OpVector mindEatOperation(const Eat & op);
    OpVector mindFireOperation(const Fire & op);
    OpVector mindGetOperation(const Get & op);
    OpVector mindImaginaryOperation(const Imaginary & op);
    OpVector mindInfoOperation(const Info & op);
    OpVector mindMoveOperation(const Move & op);
    OpVector mindNourishOperation(const Nourish & op);
    OpVector mindSetOperation(const Set & op);
    OpVector mindSightOperation(const Sight & op);
    OpVector mindSoundOperation(const Sound & op);
    OpVector mindTalkOperation(const Talk & op);
    OpVector mindTickOperation(const Tick & op);
    OpVector mindTouchOperation(const Touch & op);
    OpVector mindLookOperation(const Look & op);
    OpVector mindLoadOperation(const Load & op);
    OpVector mindSaveOperation(const Save & op);
    OpVector mindSetupOperation(const Setup & op);
    OpVector mindAppearanceOperation(const Appearance & op);
    OpVector mindDisappearanceOperation(const Disappearance & op);
    OpVector mindErrorOperation(const Error & op);
    OpVector mindOtherOperation(const RootOperation & op);

    bool w2mLoginOperation(const Login & op);
    bool w2mActionOperation(const Action & op);
    bool w2mChopOperation(const Chop & op);
    bool w2mCombineOperation(const Combine & op);
    bool w2mCreateOperation(const Create & op);
    bool w2mCutOperation(const Cut & op);
    bool w2mDeleteOperation(const Delete & op);
    bool w2mDivideOperation(const Divide & op);
    bool w2mEatOperation(const Eat & op);
    bool w2mFireOperation(const Fire & op);
    bool w2mGetOperation(const Get & op);
    bool w2mImaginaryOperation(const Imaginary & op);
    bool w2mInfoOperation(const Info & op);
    bool w2mMoveOperation(const Move & op);
    bool w2mNourishOperation(const Nourish & op);
    bool w2mSetOperation(const Set & op);
    bool w2mSightOperation(const Sight & op);
    bool w2mSoundOperation(const Sound & op);
    bool w2mTouchOperation(const Touch & op);
    bool w2mTickOperation(const Tick & op);
    bool w2mLookOperation(const Look & op);
    bool w2mLoadOperation(const Load & op);
    bool w2mSaveOperation(const Save & op);
    bool w2mSetupOperation(const Setup & op);
    bool w2mTalkOperation(const Talk & op);
    bool w2mAppearanceOperation(const Appearance & op);
    bool w2mDisappearanceOperation(const Disappearance & op);
    bool w2mErrorOperation(const Error & op);
    bool w2mOtherOperation(const RootOperation & op);

    OpVector sendMind(const RootOperation & op);
    OpVector mind2body(const RootOperation & op);
    OpVector world2body(const RootOperation & op);
    bool world2mind(const RootOperation & op);
};

#endif // RULESETS_CHARACTER_H
