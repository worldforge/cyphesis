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


#ifndef RULESETS_CHARACTER_H
#define RULESETS_CHARACTER_H

#include "Thing.h"

class BaseMind;
class ExternalMind;
class Link;
class Movement;
class Task;

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
class Character : public Thing {
  protected:
    /// \brief Handler for simulating movement under direction from the mind
    Movement & m_movement;

    /// \brief Energy loss by metabolism per tick
    static const double energyConsumption;
    /// \brief Food consumed by digestion per tick
    static const double foodConsumption;
    /// \brief Proportion of weight converted to energy per tick when starving
    static const double weightConsumption;
    /// \brief Energy converted to weight by metabolism per tick
    static const double energyLaidDown;
    /// \brief Weight gained from excess energy by metabolism per tick
    static const double weightGain;

    void filterExternalOperation(const Operation &);
    void metabolise(OpVector &, double ammount = 1); 
    void wieldDropped();
    LocatedEntity * findInContains(LocatedEntity * ent, const std::string & id);
    LocatedEntity * findInInventory(const std::string & id);

    friend class Movement;
  public:
    /// \brief Internal AI mind controlling this character
    BaseMind * m_mind;
    /// \brief External network connected agent controlling this character
    ExternalMind * m_externalMind;

    explicit Character(const std::string & id, long intId);
    virtual ~Character();

    int linkExternal(Link *);
    int unlinkExternal(Link *);

    int startTask(Task *, const Operation & op, OpVector &);
    void updateTask(OpVector &);
    void clearTask(OpVector &);

    virtual void operation(const Operation & op, OpVector &);
    virtual void externalOperation(const Operation & op, Link &);

    virtual void ImaginaryOperation(const Operation & op, OpVector &);
    virtual void InfoOperation(const Operation & op, OpVector &);
    virtual void TickOperation(const Operation & op, OpVector &);
    virtual void TalkOperation(const Operation & op, OpVector &);
    virtual void NourishOperation(const Operation & op, OpVector &);
    virtual void UseOperation(const Operation & op, OpVector &);
    virtual void WieldOperation(const Operation & op, OpVector &);
    virtual void AttackOperation(const Operation & op, OpVector &);
    virtual void ActuateOperation(const Operation & op, OpVector &);

    virtual void mindActuateOperation(const Operation &, OpVector &);
    virtual void mindAttackOperation(const Operation &, OpVector &);
    virtual void mindCombineOperation(const Operation &, OpVector &);
    virtual void mindCreateOperation(const Operation &, OpVector &);
    virtual void mindDeleteOperation(const Operation &, OpVector &);
    virtual void mindDivideOperation(const Operation &, OpVector &);
    virtual void mindEatOperation(const Operation &, OpVector &);
    virtual void mindGoalInfoOperation(const Operation &, OpVector &);
    virtual void mindImaginaryOperation(const Operation &, OpVector &);
    virtual void mindLookOperation(const Operation &, OpVector &);
    virtual void mindMoveOperation(const Operation &, OpVector &);
    virtual void mindSetOperation(const Operation &, OpVector &);
    virtual void mindSetupOperation(const Operation &, OpVector &);
    virtual void mindTalkOperation(const Operation &, OpVector &);
    virtual void mindThoughtOperation(const Operation &, OpVector &);
    virtual void mindTickOperation(const Operation &, OpVector &);
    virtual void mindTouchOperation(const Operation &, OpVector &);
    virtual void mindUpdateOperation(const Operation &, OpVector &);
    virtual void mindUseOperation(const Operation &, OpVector &);
    virtual void mindWieldOperation(const Operation &, OpVector &);

    virtual void mindOtherOperation(const Operation &, OpVector &);

    bool w2mSightOperation(const Operation &);
    bool w2mSoundOperation(const Operation &);
    bool w2mTouchOperation(const Operation &);
    bool w2mTickOperation(const Operation &);
    bool w2mUnseenOperation(const Operation &);
    bool w2mSetupOperation(const Operation &);
    bool w2mAppearanceOperation(const Operation &);
    bool w2mDisappearanceOperation(const Operation &);
    bool w2mErrorOperation(const Operation &);
    bool w2mThoughtOperation(const Operation & op);

    void sendMind(const Operation & op, OpVector &);
    void mind2body(const Operation & op, OpVector &);
    bool world2mind(const Operation & op);

    friend class Charactertest;
};

#endif // RULESETS_CHARACTER_H
