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
    bool m_isAlive;
    std::string m_rightHandWield;

    // Properties
    double m_drunkness;
    std::string m_sex;
    double m_food;
    double m_maxMass;

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

    virtual void operation(const RootOperation & op, OpVector &);
    virtual void externalOperation(const RootOperation & op);

    virtual void ImaginaryOperation(const RootOperation & op, OpVector &);
    virtual void SetupOperation(const RootOperation & op, OpVector &);
    virtual void TickOperation(const RootOperation & op, OpVector &);
    virtual void TalkOperation(const RootOperation & op, OpVector &);
    virtual void EatOperation(const RootOperation & op, OpVector &);
    virtual void NourishOperation(const RootOperation & op, OpVector &);
    virtual void WieldOperation(const RootOperation & op, OpVector &);

    virtual void mindLoginOperation(const RootOperation &, OpVector &);
    virtual void mindLogoutOperation(const RootOperation &, OpVector &);
    virtual void mindCreateOperation(const RootOperation &, OpVector &);
    virtual void mindActionOperation(const RootOperation &, OpVector &);
    virtual void mindChopOperation(const RootOperation &, OpVector &);
    virtual void mindCombineOperation(const RootOperation &, OpVector &);
    virtual void mindCutOperation(const RootOperation &, OpVector &);
    virtual void mindDeleteOperation(const RootOperation &, OpVector &);
    virtual void mindDivideOperation(const RootOperation &, OpVector &);
    virtual void mindEatOperation(const RootOperation &, OpVector &);
    virtual void mindBurnOperation(const RootOperation &, OpVector &);
    virtual void mindGetOperation(const RootOperation &, OpVector &);
    virtual void mindImaginaryOperation(const RootOperation &, OpVector &);
    virtual void mindInfoOperation(const RootOperation &, OpVector &);
    virtual void mindMoveOperation(const RootOperation &, OpVector &);
    virtual void mindNourishOperation(const RootOperation &, OpVector &);
    virtual void mindSetOperation(const RootOperation &, OpVector &);
    virtual void mindSightOperation(const RootOperation &, OpVector &);
    virtual void mindSoundOperation(const RootOperation &, OpVector &);
    virtual void mindTalkOperation(const RootOperation &, OpVector &);
    virtual void mindTickOperation(const RootOperation &, OpVector &);
    virtual void mindTouchOperation(const RootOperation &, OpVector &);
    virtual void mindLookOperation(const RootOperation &, OpVector &);
    virtual void mindSetupOperation(const RootOperation &, OpVector &);
    virtual void mindAppearanceOperation(const RootOperation &, OpVector &);
    virtual void mindDisappearanceOperation(const RootOperation &, OpVector &);
    virtual void mindUseOperation(const RootOperation &, OpVector &);
    virtual void mindWieldOperation(const RootOperation &, OpVector &);
    virtual void mindErrorOperation(const RootOperation &, OpVector &);
    virtual void mindOtherOperation(const RootOperation &, OpVector &);

    bool w2mLoginOperation(const RootOperation &);
    bool w2mLogoutOperation(const RootOperation &);
    bool w2mActionOperation(const RootOperation &);
    bool w2mChopOperation(const RootOperation &);
    bool w2mCombineOperation(const RootOperation &);
    bool w2mCreateOperation(const RootOperation &);
    bool w2mCutOperation(const RootOperation &);
    bool w2mDeleteOperation(const RootOperation &);
    bool w2mDivideOperation(const RootOperation &);
    bool w2mEatOperation(const RootOperation &);
    bool w2mBurnOperation(const RootOperation &);
    bool w2mGetOperation(const RootOperation &);
    bool w2mImaginaryOperation(const RootOperation &);
    bool w2mInfoOperation(const RootOperation &);
    bool w2mMoveOperation(const RootOperation &);
    bool w2mNourishOperation(const RootOperation &);
    bool w2mSetOperation(const RootOperation &);
    bool w2mSightOperation(const RootOperation &);
    bool w2mSoundOperation(const RootOperation &);
    bool w2mTouchOperation(const RootOperation &);
    bool w2mTickOperation(const RootOperation &);
    bool w2mLookOperation(const RootOperation &);
    bool w2mSetupOperation(const RootOperation &);
    bool w2mTalkOperation(const RootOperation &);
    bool w2mAppearanceOperation(const RootOperation &);
    bool w2mDisappearanceOperation(const RootOperation &);
    bool w2mUseOperation(const RootOperation &);
    bool w2mWieldOperation(const RootOperation &);
    bool w2mErrorOperation(const RootOperation &);
    bool w2mOtherOperation(const RootOperation &);

    void sendMind(const RootOperation & op, OpVector &);
    void mind2body(const RootOperation & op, OpVector &);
    bool world2mind(const RootOperation & op);
};

#endif // RULESETS_CHARACTER_H
