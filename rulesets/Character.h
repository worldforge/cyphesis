// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_CHARACTER_H
#define RULESETS_CHARACTER_H

#include "Thing.h"

class Movement;
class BaseMind;

typedef Thing Character_parent;

/// \brief This is the class for anything controlled by an AI mind, or
/// external controller like a player client.
///
/// It currently contains large ammounts of code for handling movement
/// which should probably be moved into Thing (PhysicalEntity).
/// This class currently assumes it is used to handle all animals,
/// so handles food, nourishment and intoxication.
/// The mind interface includes operation processing for operations from
/// the mind or client to determine what the result on the world is, and
/// operation verification for checking if an in-game operation should be
/// passed to the mind.
class Character : public Character_parent {
  protected:
    Movement & m_movement;
    double m_drunkness;
    std::string m_sex;
    double m_food;
    double m_maxMass;
    bool m_isAlive;

    static const double energyConsumption = 0.001;
    static const double foodConsumption = 0.1;
    static const double weightConsumption = 1.0;
    static const double energyGain = 0.5;
    static const double energyLoss = 0.1;
    static const double weightGain = 0.5;

    void metabolise(OpVector &, double ammount = 1); 

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
    BaseMind * m_mind;
    BaseEntity * m_externalMind;

    explicit Character(const std::string & id);
    virtual ~Character();

    const double getDrunkness() const { return m_drunkness; }
    const std::string & getSex() const { return m_sex; }
    const double getFood() const { return m_food; }

    virtual bool get(const std::string &, Element &) const;
    virtual void set(const std::string &, const Element &);

    virtual void addToMessage(MapType &) const;

    virtual void operation(const RootOperation & op, OpVector &);
    virtual void externalOperation(const RootOperation & op);

    virtual void ImaginaryOperation(const Imaginary & op, OpVector &);
    virtual void SetupOperation(const Setup & op, OpVector &);
    virtual void TickOperation(const Tick & op, OpVector &);
    virtual void TalkOperation(const Talk & op, OpVector &);
    virtual void EatOperation(const Eat & op, OpVector &);
    virtual void NourishOperation(const Nourish & op, OpVector &);

    virtual void mindLoginOperation(const Login &, OpVector &);
    virtual void mindLogoutOperation(const Logout &, OpVector &);
    virtual void mindCreateOperation(const Create &, OpVector &);
    virtual void mindActionOperation(const Action &, OpVector &);
    virtual void mindChopOperation(const Chop &, OpVector &);
    virtual void mindCombineOperation(const Combine &, OpVector &);
    virtual void mindCutOperation(const Cut &, OpVector &);
    virtual void mindDeleteOperation(const Delete &, OpVector &);
    virtual void mindDivideOperation(const Divide &, OpVector &);
    virtual void mindEatOperation(const Eat &, OpVector &);
    virtual void mindBurnOperation(const Burn &, OpVector &);
    virtual void mindGetOperation(const Get &, OpVector &);
    virtual void mindImaginaryOperation(const Imaginary &, OpVector &);
    virtual void mindInfoOperation(const Info &, OpVector &);
    virtual void mindMoveOperation(const Move &, OpVector &);
    virtual void mindNourishOperation(const Nourish &, OpVector &);
    virtual void mindSetOperation(const Set &, OpVector &);
    virtual void mindSightOperation(const Sight &, OpVector &);
    virtual void mindSoundOperation(const Sound &, OpVector &);
    virtual void mindTalkOperation(const Talk &, OpVector &);
    virtual void mindTickOperation(const Tick &, OpVector &);
    virtual void mindTouchOperation(const Touch &, OpVector &);
    virtual void mindLookOperation(const Look &, OpVector &);
    virtual void mindSetupOperation(const Setup &, OpVector &);
    virtual void mindAppearanceOperation(const Appearance &, OpVector &);
    virtual void mindDisappearanceOperation(const Disappearance &, OpVector &);
    virtual void mindUseOperation(const Use &, OpVector &);
    virtual void mindWieldOperation(const Wield &, OpVector &);
    virtual void mindErrorOperation(const Error &, OpVector &);
    virtual void mindOtherOperation(const RootOperation &, OpVector &);

    bool w2mLoginOperation(const Login &);
    bool w2mLogoutOperation(const Logout &);
    bool w2mActionOperation(const Action &);
    bool w2mChopOperation(const Chop &);
    bool w2mCombineOperation(const Combine &);
    bool w2mCreateOperation(const Create &);
    bool w2mCutOperation(const Cut &);
    bool w2mDeleteOperation(const Delete &);
    bool w2mDivideOperation(const Divide &);
    bool w2mEatOperation(const Eat &);
    bool w2mBurnOperation(const Burn &);
    bool w2mGetOperation(const Get &);
    bool w2mImaginaryOperation(const Imaginary &);
    bool w2mInfoOperation(const Info &);
    bool w2mMoveOperation(const Move &);
    bool w2mNourishOperation(const Nourish &);
    bool w2mSetOperation(const Set &);
    bool w2mSightOperation(const Sight &);
    bool w2mSoundOperation(const Sound &);
    bool w2mTouchOperation(const Touch &);
    bool w2mTickOperation(const Tick &);
    bool w2mLookOperation(const Look &);
    bool w2mSetupOperation(const Setup &);
    bool w2mTalkOperation(const Talk &);
    bool w2mAppearanceOperation(const Appearance &);
    bool w2mDisappearanceOperation(const Disappearance &);
    bool w2mUseOperation(const Use &);
    bool w2mWieldOperation(const Wield &);
    bool w2mErrorOperation(const Error &);
    bool w2mOtherOperation(const RootOperation &);

    void sendMind(const RootOperation & op, OpVector &);
    void mind2body(const RootOperation & op, OpVector &);
    bool world2mind(const RootOperation & op);
};

#endif // RULESETS_CHARACTER_H
