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
#include <sigc++/connection.h>
#include <sigc++/trackable.h>

class ProxyMind;
class ExternalMind;
class Link;
class Movement;
class Task;



/// \brief This is the class for anything controlled by an AI mind, or
/// external controller like a player client.
///
/// It currently contains large amounts of code for handling movement
/// which should probably be moved into Thing (PhysicalEntity).
/// This class currently assumes it is used to handle all animals,
/// so handles food, nourishment and intoxication.
/// The mind interface includes operation processing for operations from
/// the mind or client to determine what the result on the world is, and
/// operation verification for checking if an in-game operation should be
/// passed to the mind.
/// \ingroup EntityClasses
class Character : public Thing, public virtual sigc::trackable {

  public:

    /// \brief Stores relay data, as generated by a Relay operation.
    ///
    /// Whenever a Relay operation is used it's with the intention that any response
    /// should be relayed back to the originating entity (using serialno and refno).
    /// This struct is used to keep track of this routing data, so that the operation
    /// can be correctly sent on to its destination.
    struct Relay
    {
            /// The entity/router id to which the op was sent or should be sent.
            std::string destination;

            /// The original serial no which the relayed operation should have.
            long int serialno;
    };


  protected:
    /// \brief Handler for simulating movement under direction from the mind
    Movement & m_movement;
    /// \brief Internal AI mind keeping track of what this character experiences.
    ProxyMind * m_proxyMind;


    /**
     * \brief A store of registered relays for this character, both outgoing and incoming.
     *
     * Whenever a relay is set up between two entities entries are created in this
     * map. The refno of the operations is used as a key.
     */
    std::map<long int, Relay> m_relays;

    /**
     * Serial number generation to use when generating new serial numbers.
     */
    static long int s_serialNumberNext;

    /// \brief Holds a connection to the containered signal of any wielded entity.
    ///
    /// FIXME This is a hack, to be removed once we've migrated to using Outfit
    /// for wielded entities.
    sigc::connection m_rightHandWieldConnection;

    void filterExternalOperation(const Operation &);
    void wieldDropped();
    LocatedEntity * findInContains(LocatedEntity * ent, const std::string & id);
    LocatedEntity * findInInventory(const std::string & id);

    friend class Movement;
  public:
    /// \brief External network connected agent controlling this character
    ExternalMind * m_externalMind;

    /// \brief Emitted when the external link for this character has changed.
    sigc::signal<void> externalLinkChanged;

    explicit Character(const std::string & id, long intId);
    virtual ~Character();

    int linkExternal(Link *);
    int unlinkExternal(Link *);

    int startTask(Task *, const Operation & op, OpVector &);
    void updateTask(OpVector &);
    void clearTask(OpVector &);

    virtual std::vector<Atlas::Objects::Root> getThoughts() const;

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
    virtual void RelayOperation(const Operation &, OpVector &);

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
    virtual void mindThinkOperation(const Operation &, OpVector &);
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
    bool w2mThinkOperation(const Operation & op);
    bool w2mCommuneOperation(const Operation & op);
    bool w2mRelayOperation(const Operation & op);

    void sendMind(const Operation & op, OpVector &);
    void mind2body(const Operation & op, OpVector &);
    bool world2mind(const Operation & op);

    friend class Charactertest;
};

#endif // RULESETS_CHARACTER_H
