// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Character.h,v 1.91 2007-01-03 22:48:03 alriddoch Exp $

#ifndef RULESETS_CHARACTER_H
#define RULESETS_CHARACTER_H

#include "Thing.h"

#include "Statistics.h"

#include "modules/EntityRef.h"

#include <sigc++/connection.h>

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
/// \ingroup EntityClasses
class Character : public Character_parent {
  protected:
    /// \brief Mathematical model that calculates characters stats and abilities
    Statistics m_statistics;
    /// \brief Handler for simulating movement under direction from the mind
    Movement & m_movement;
    /// \brief Handler for a medium term task being undertaken
    Task * m_task;
    /// \brief Flag indicating if this entity is alive
    bool m_isAlive;

    // Properties
    /// \brief Level of stamina character has left
    double m_stamina;
    /// \brief Ammount of food available for digestion
    double m_food;
    /// \brief Maximum mass of this type of creature
    double m_maxMass;

    /// \brief Identifier of entity in the characters right hand
    EntityRef m_rightHandWield;
    /// \brief Sigc connection used to ensure the entity is removed
    /// when it changes containers.
    sigc::connection m_rightHandWieldConnection;

    /// \brief Energy converted to weight by metabolism per tick
    static const double energyConsumption;
    /// \brief Food consumed by digestion per tick
    static const double foodConsumption;
    /// \brief Weight converted to energy per tick
    static const double weightConsumption;
    /// \brief Energy gained from digestion per tick
    static const double energyGain;
    /// \brief Energy loss by metabolism per tick
    static const double energyLoss;
    /// \brief Weight gained from excess energy by metabolism per tick
    static const double weightGain;

    void metabolise(OpVector &, double ammount = 1); 
    void wieldDropped();
    Entity * findInContains(Entity * ent, const std::string & id);
    Entity * findInInventory(const std::string & id);

    friend class Movement;
  public:
    /// \brief Internal AI mind controlling this character
    BaseMind * m_mind;
    /// \brief External network connected agent controlling this character
    BaseEntity * m_externalMind;

    explicit Character(const std::string & id, long intId);
    virtual ~Character();

    /// \brief Accessor for mathematical stats model
    Statistics & statistics() { return m_statistics; }
    /// \brief Accessor for medium term task
    Task * task() { return m_task; }

    /// \brief Accessor for raw stamina property
    const double getStamina() const { return m_stamina; }
    /// \brief Accessor for raw food property
    const double getFood() const { return m_food; }
    /// \brief Accessor for raw right hand wield property
    const std::string & getRightHandWield() const {
        return m_rightHandWield->getId();
    }

    void setTask(Task *);
    void updateTask();
    void clearTask();

    virtual void operation(const Operation & op, OpVector &);
    virtual void externalOperation(const Operation & op);

    virtual void ImaginaryOperation(const Operation & op, OpVector &);
    virtual void SetupOperation(const Operation & op, OpVector &);
    virtual void TickOperation(const Operation & op, OpVector &);
    virtual void TalkOperation(const Operation & op, OpVector &);
    virtual void EatOperation(const Operation & op, OpVector &);
    virtual void NourishOperation(const Operation & op, OpVector &);
    virtual void WieldOperation(const Operation & op, OpVector &);
    virtual void AttackOperation(const Operation & op, OpVector &);
    virtual void ChopOperation(const Operation & op, OpVector &);

    virtual void mindActionOperation(const Operation &, OpVector &);
    virtual void mindAddOperation(const Operation &, OpVector &);
    virtual void mindAppearanceOperation(const Operation &, OpVector &);
    virtual void mindAttackOperation(const Operation &, OpVector &);
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
    bool w2mAttackOperation(const Operation &);
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
    bool w2mUnseenOperation(const Operation &);
    bool w2mUpdateOperation(const Operation &);
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
