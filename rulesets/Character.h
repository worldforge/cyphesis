// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_CHARACTER_H
#define RULESETS_CHARACTER_H

#include "Thing.h"

class Movement;
class Task;
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
    Task * m_task;
    bool m_isAlive;

    // Properties
    double m_drunkness;
    std::string m_sex;
    double m_food;
    double m_maxMass;
    std::string m_rightHandWield;

    static const double energyConsumption;
    static const double foodConsumption;
    static const double weightConsumption;
    static const double energyGain;
    static const double energyLoss;
    static const double weightGain;

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
    const std::string & getRightHandWield() const { return m_rightHandWield; }

    virtual void operation(const Operation & op, OpVector &);
    virtual void externalOperation(const Operation & op);

    virtual void ImaginaryOperation(const Operation & op, OpVector &);
    virtual void SetupOperation(const Operation & op, OpVector &);
    virtual void TickOperation(const Operation & op, OpVector &);
    virtual void TalkOperation(const Operation & op, OpVector &);
    virtual void EatOperation(const Operation & op, OpVector &);
    virtual void NourishOperation(const Operation & op, OpVector &);
    virtual void WieldOperation(const Operation & op, OpVector &);

    virtual void mindActionOperation(const Operation &, OpVector &);
    virtual void mindAddOperation(const Operation &, OpVector &);
    virtual void mindAppearanceOperation(const Operation &, OpVector &);
    virtual void mindBurnOperation(const Operation &, OpVector &);
    virtual void mindChopOperation(const Operation &, OpVector &);
    virtual void mindCombineOperation(const Operation &, OpVector &);
    virtual void mindCreateOperation(const Operation &, OpVector &);
    virtual void mindCutOperation(const Operation &, OpVector &);
    virtual void mindDeleteOperation(const Operation &, OpVector &);
    virtual void mindDisappearanceOperation(const Operation &, OpVector &);
    virtual void mindDivideOperation(const Operation &, OpVector &);
    virtual void mindEatOperation(const Operation &, OpVector &);
    virtual void mindGetOperation(const Operation &, OpVector &);
    virtual void mindImaginaryOperation(const Operation &, OpVector &);
    virtual void mindInfoOperation(const Operation &, OpVector &);
    virtual void mindLoginOperation(const Operation &, OpVector &);
    virtual void mindLogoutOperation(const Operation &, OpVector &);
    virtual void mindLookOperation(const Operation &, OpVector &);
    virtual void mindMoveOperation(const Operation &, OpVector &);
    virtual void mindNourishOperation(const Operation &, OpVector &);
    virtual void mindSetOperation(const Operation &, OpVector &);
    virtual void mindSetupOperation(const Operation &, OpVector &);
    virtual void mindSightOperation(const Operation &, OpVector &);
    virtual void mindSoundOperation(const Operation &, OpVector &);
    virtual void mindTalkOperation(const Operation &, OpVector &);
    virtual void mindTickOperation(const Operation &, OpVector &);
    virtual void mindTouchOperation(const Operation &, OpVector &);
    virtual void mindUpdateOperation(const Operation &, OpVector &);
    virtual void mindUseOperation(const Operation &, OpVector &);
    virtual void mindWieldOperation(const Operation &, OpVector &);

    virtual void mindErrorOperation(const Operation &, OpVector &);
    virtual void mindOtherOperation(const Operation &, OpVector &);

    bool w2mLoginOperation(const Operation &);
    bool w2mLogoutOperation(const Operation &);
    bool w2mActionOperation(const Operation &);
    bool w2mChopOperation(const Operation &);
    bool w2mCombineOperation(const Operation &);
    bool w2mCreateOperation(const Operation &);
    bool w2mCutOperation(const Operation &);
    bool w2mDeleteOperation(const Operation &);
    bool w2mDivideOperation(const Operation &);
    bool w2mEatOperation(const Operation &);
    bool w2mBurnOperation(const Operation &);
    bool w2mGetOperation(const Operation &);
    bool w2mImaginaryOperation(const Operation &);
    bool w2mInfoOperation(const Operation &);
    bool w2mMoveOperation(const Operation &);
    bool w2mNourishOperation(const Operation &);
    bool w2mSetOperation(const Operation &);
    bool w2mSightOperation(const Operation &);
    bool w2mSoundOperation(const Operation &);
    bool w2mTouchOperation(const Operation &);
    bool w2mTickOperation(const Operation &);
    bool w2mLookOperation(const Operation &);
    bool w2mSetupOperation(const Operation &);
    bool w2mTalkOperation(const Operation &);
    bool w2mAppearanceOperation(const Operation &);
    bool w2mDisappearanceOperation(const Operation &);
    bool w2mUseOperation(const Operation &);
    bool w2mWieldOperation(const Operation &);
    bool w2mErrorOperation(const Operation &);
    bool w2mOtherOperation(const Operation &);

    void sendMind(const Operation & op, OpVector &);
    void mind2body(const Operation & op, OpVector &);
    bool world2mind(const Operation & op);
};

#endif // RULESETS_CHARACTER_H
