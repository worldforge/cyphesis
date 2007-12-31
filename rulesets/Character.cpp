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

// $Id: Character.cpp,v 1.309 2007-12-31 17:39:26 alriddoch Exp $

#include "Character.h"

#include "Pedestrian.h"
#include "MindFactory.h"
#include "BaseMind.h"
#include "Script.h"
#include "World.h"
#include "Task.h"
#include "StatisticsProperty.h"
#include "EntityProperty.h"
#include "OutfitProperty.h"

#include "common/op_switch.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/TypeNode.h"
#include "common/serialno.h"
#include "common/compose.hpp"
#include "common/PropertyManager.h"

#include "common/Add.h"
#include "common/Attack.h"
#include "common/Eat.h"
#include "common/Nourish.h"
#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"
#include "common/Update.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

// This figure is calculated to allow a character to live for 4 weeks
// without food, during which time they will lose 40% of their mass
// before starving.
const double Character::energyConsumption = 0.0001;

// Food consumption is fast, to keep Acorn playable
const double Character::foodConsumption = 0.1;

// When the character is starving, they start to lose weight. During
// the latter 2 and a half weeks, they lose about 40% of their mass.
// Bad players who do not feed their characters will be punished.
const double Character::weightConsumption = 0.00002;

// Ammount of evergy turned into weight by metabolism when Character
// is well fed.
const double Character::energyLaidDown = 0.1;

// Ammount of weight gained as a result. High for Acorn.
const double Character::weightGain = 0.5;


/// \brief Calculate how the Characters metabolism has affected it in the
/// last tick
///
/// This function is called every 90 seconds. It does one of three things.
/// If energy is very high, it loses some, and gains some weight. Otherwise
/// it loses some energy, unless energy is very low, in which case loss
/// is slower, as weight is used to compensate.
/// A fully healthy Character should take about a week to starve to death.
/// So 10080 / 90 = 6720 ticks.
/// @param res Any result of changes is returned here.
/// @param ammount Time scale factor, currently not used.
void Character::metabolise(OpVector & res, double ammount)
{
    bool stamina_changed = false,
         mass_changed = false;

    // Currently handles energy
    // We should probably call this whenever the entity performs a movement.

    PropertyBase * status_prop = getProperty("status");
    if (status_prop == 0) {
        status_prop = PropertyManager::instance()->addProperty(this, "status");
        if (status_prop == 0) {
            log(ERROR, "Unable to get a STATUS property.");
            return;
        }
        m_properties["status"] = status_prop;
        status_prop->set(1.f);
    }
    Element value;
    status_prop->get(value);
    assert(value.isFloat());
    float status = value.asFloat();

    Anonymous update_arg;
    update_arg->setId(getId());

    PropertyBase * food_prop = getProperty("food");
    // DIGEST
    if (food_prop != 0 && food_prop->get(value) && value.isFloat()) {
        float food = value.Float();
        if (food >= foodConsumption && status < 2) {
            // It is important that the metabolise bit is done next, as this
            // handles the status change
            status += foodConsumption;
            food -= foodConsumption;
            food_prop->set(food);

            update_arg->setAttr("food", food);
        }
    }

    PropertyBase * mass_prop = getProperty("mass");
    float mass = 0.f;
    if (mass_prop != 0 && mass_prop->get(value) && value.isFloat()) {
        mass = value.Float();
    }
    // If status is very high, we gain weight
    if (status > (1.5 + energyLaidDown) && mass < m_maxMass) {
        status -= energyLaidDown;
        mass += weightGain;
        mass_changed = true;
    } else {
        // If status is relatively is not very high, then energy is burned
        double energy_used = energyConsumption * ammount;
        double weight_used = weightConsumption * mass * ammount;
        if (status <= 0.5 && mass > weight_used) {
            // Drain away a little energy and lose some weight
            // This ensures there is a long term penalty to allowing something
            // to starve
            status -= (energy_used / 2);
            mass -= weight_used;
            mass_changed = true;
        } else {
            // Just drain away a little energy
            status -= energy_used;
        }
    }
    if (m_stamina < 1. && m_task == 0 && !m_movement.updateNeeded(m_location)) {
        m_stamina = 1.;
        stamina_changed = true;
    }

    status_prop->set(status);
    update_arg->setAttr("status", status);

    if (mass_changed && mass_prop != 0) {
        mass_prop->set(mass);
        update_arg->setAttr("mass", mass);
    }
    if (stamina_changed) {
        update_arg->setAttr("stamina", m_stamina);
    }

    Update update;
    update->setTo(getId());
    update->setArgs1(update_arg);

    res.push_back(update);
}

/// \brief Hooked to the Entity::containered signal of the wielded entity
/// to indicate a change of location
///
/// This function responds by removing it as a wielded entity.
void Character::wieldDropped()
{
    Wield wield;
    wield->setTo(getId());
    sendWorld(wield);
}

/// \brief Search for an entity in an entities contents
///
/// Recursive function the finds an entity by ID in another entities
/// contains list.
/// @param ent Entity to search in
/// @param id Identifier of entity to search for
LocatedEntity * Character::findInContains(LocatedEntity * ent,
                                          const std::string & id)
{
    LocatedEntitySet::const_iterator I = ent->m_contains->begin();
    LocatedEntitySet::const_iterator Iend = ent->m_contains->end();
    for (; I != Iend; ++I) {
        LocatedEntity * child = *I;
        if (child->getId() == id) {
            return *I;
        }
        if (!child->m_contains->empty()) {
            LocatedEntity * found = findInContains(child, id);
            if (found != 0) {
                return found;
            }
        }
    }
    return 0;
}

/// \brief Search for an entity in the Character's inventory
///
/// Implemented using the recursive function findInContains.
/// @param id Identifier of entity to search for
LocatedEntity * Character::findInInventory(const std::string & id)
{
    return findInContains(this, id);
}

/// \brief Character constructor
///
/// @param id String identifier
/// @param intId Integer identifier
Character::Character(const std::string & id, long intId) :
                                            Character_parent(id, intId),
                                            m_statistics(*this),
                                            m_movement(*new Pedestrian(*this)),
                                            m_task(0), m_isAlive(true),
                                            m_stamina(1.),
                                            m_maxMass(100),
                                            m_mind(0), m_externalMind(0)
{
    m_location.setBBox(BBox(WFMath::Point<3>(-0.25, -0.25, 0),
                            WFMath::Point<3>(0.25, 0.25, 2)));

    m_properties["stamina"] = new Property<double>(m_stamina, 0);
    m_properties["statistics"] = new StatisticsProperty(m_statistics, 0);
    m_properties["right_hand_wield"] = new EntityProperty(m_rightHandWield);
}

Character::~Character()
{
    delete &m_movement;
    if (m_mind != 0) {
        delete m_mind;
    }
    if (m_externalMind != 0) {
        delete m_externalMind;
    }
}

/// \brief Set a new task as the one being performed by the Character
///
/// The old one is cleared and deleted if present
/// @param task The new task to be assigned to the Character
void Character::setTask(Task * task)
{
    if (m_task != 0) {
        clearTask();
    }
    m_task = task;
    task->incRef();

    updateTask();
}

/// \brief Update the visible representation of the current task
///
/// Generate a Set operation which modifies the Characters task property
/// to reflect the current status of the task.
void Character::updateTask()
{
    if (m_task == 0) {
        log(ERROR, "Character::updateTask called when no task running");
    }

    Anonymous set_arg;
    m_task->addToEntity(set_arg);
    set_arg->setId(getId());

    Set set;
    set->setArgs1(set_arg);
    set->setTo(getId());
    
    sendWorld(set);
}

/// \brief Clean up and remove the task currently being executed
///
/// Remove the task, and send an operation indicating that no tasks
/// are now present.
void Character::clearTask()
{
    if (m_task == 0) {
        log(ERROR, "Character.clearTask: No task currently set");
        return;
    }
    m_task->decRef();
    m_task = 0;

    Anonymous set_arg;
    set_arg->setAttr("tasks", ListType());
    set_arg->setId(getId());

    Set set;
    set->setArgs1(set_arg);
    set->setTo(getId());
    
    sendWorld(set);
}

void Character::ImaginaryOperation(const Operation & op, OpVector & res)
{
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
}

void Character::SetupOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "CHaracter::SetupOperation()" << std::endl
                     << std::flush;);

    if (!op->getArgs().empty()) {
        debug( std::cout << __func__ << " Setup op is for subsystem" << std::endl << std::flush;);
        return;
    }

    if (0 == m_externalMind) {
        // This ensures that newly created player characters don't get
        // bogged down with an NPC mind. In the short term this
        // takes away PC programmability.
        // FIXME Characters restored from the database will still get
        // AI minds, so  we need to handle them somehow differently.
        // Perhaps the Restore op (different from Setup op) is needed?

        m_mind = MindFactory::instance()->newMind(getId(), getIntId(), m_type);

        Operation s(op.copy());
        Anonymous setup_arg;
        setup_arg->setName("mind");
        s->setArgs1(setup_arg);
        res.push_back(s);

        Look l;
        l->setTo(getId());
        res.push_back(l);
    }

    Tick tick;
    tick->setTo(getId());
    res.push_back(tick);
}

void Character::TickOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "================================" << std::endl
                    << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (!args.empty()) {
        const Root & arg = args.front();
        if (arg->getName() == "move") {
            // Deal with movement.
            Element serialno;
            if (arg->copyAttr("serialno", serialno) == 0 && (serialno.isInt())) {
                if (serialno.asInt() < m_movement.serialno()) {
                    debug(std::cout << "Old tick" << std::endl << std::flush;);
                    return;
                }
            } else {
                log(ERROR, "Character::TickOperation: No serialno in tick arg");
            }
            Location return_location;
            if (m_movement.getUpdatedLocation(return_location)) {
                return;
            }
            res.push_back(m_movement.generateMove(return_location));
            Anonymous tick_arg;
            tick_arg->setName("move");
            tick_arg->setAttr("serialno", m_movement.serialno());
            Tick tickOp;
            tickOp->setTo(getId());
            tickOp->setFutureSeconds(m_movement.getTickAddition(return_location.pos(), return_location.velocity()));
            tickOp->setArgs1(tick_arg);
            res.push_back(tickOp);
        } else if (arg->getName() == "task") {
            // Deal with task iteration
            if (m_task == 0) {
                return;
            }
            Element serialno;
            if (arg->copyAttr("serialno", serialno) == 0 && (serialno.isInt())) {
                if (serialno.asInt() != m_task->serialno()) {
                    debug(std::cout << "Old tick" << std::endl << std::flush;);
                    return;
                }
            } else {
                log(ERROR, "Character::TickOperation: No serialno in tick arg");
                return;
            }
            m_task->TickOperation(op, res);
            assert(m_task != 0);
            if (m_task->obsolete()) {
                clearTask();
            } else {
                updateTask();
            }
        } else if (arg->getName() == "mind") {
            // Do nothing. Passed to mind.
        } else {
            debug(std::cout << "Tick to unknown subsystem " << arg->getName()
                            << " arrived" << std::endl << std::flush;);
        }
    } else {
        // METABOLISE
        metabolise(res);
        
        // TICK
        Tick tickOp;
        tickOp->setTo(getId());
        tickOp->setFutureSeconds(consts::basic_tick * 30);
        res.push_back(tickOp);
    }
}

void Character::TalkOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::OPeration(Talk)" << std::endl<<std::flush;);
    Sound s;
    s->setArgs1(op);
    res.push_back(s);
}

void Character::NourishOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        error(op, "Nourish has no argument", res, getId());
        return;
    }
    const Root & arg = op->getArgs().front();
    Element mass_attr;
    if (arg->copyAttr("mass", mass_attr) != 0 || !mass_attr.isNum()) {
        return;
    }

    PropertyBase * food_property = getProperty("food");
    if (food_property == 0) {
        food_property = PropertyManager::instance()->addProperty(this, "food");
        if (food_property == 0) {
            log(ERROR, "Unable to set a FOOD property.");
            return;
        }
        m_properties["food"] = food_property;
    }
    Element value;
    food_property->get(value);
    assert(value.isFloat());
    float food = value.Float();
    food = food + mass_attr.asNum();
    food_property->set(food);

    Anonymous food_ent;
    food_ent->setId(getId());
    food_ent->setAttr("food", food);

    Set s;
    s->setArgs1(food_ent);
    // FIXME FROM, SECONDS?

    Sight si;
    si->setTo(getId());
    si->setArgs1(s);
    res.push_back(si);
}

void Character::WieldOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        // Wield nothing
        if (m_rightHandWieldConnection.connected()) {
            m_rightHandWieldConnection.disconnect();
        }

        m_rightHandWield = EntityRef(0);

        Update update;
        update->setTo(getId());
        Anonymous update_arg;
        update_arg->setId(getId());
        update_arg->setAttr("right_hand_wield", "");
        update->setArgs1(update_arg);
        res.push_back(update);

        return;
    }
    const Root & arg = op->getArgs().front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Wield arg has no ID", res, getId());
        return;
    }
    const std::string & id = arg->getId();
    Entity * item = BaseWorld::instance().getEntity(id);
    if (item == 0) {
        error(op, "Wield arg does not exist", res, getId());
        return;
    }

    LocatedEntitySet::const_iterator K = m_contains->find(item);
    if (K == m_contains->end()) {
        error(op, "Wield arg is not in inventory", res, getId());
        return;
    }

    Anonymous update_arg;
    update_arg->setId(getId());

    Element worn_attr;
    if (item->getAttr("worn", worn_attr)) {
        debug(std::cout << "Got wield for a garment" << std::endl << std::flush;);
        
        if (worn_attr.isString()) {
            OutfitProperty * outfit;
            PropertyBase * prop = getProperty("outfit");
            if (prop != 0) {
                outfit = dynamic_cast<OutfitProperty*>(prop);
                assert(outfit != 0);
            } else {
                // FIXME #8 really hacked in, should use manager
                outfit = new OutfitProperty;
                m_properties["outfit"] = outfit;
            }
            outfit->wear(this, worn_attr.String(), item);
            outfit->cleanUp();

            update_arg->setAttr("outfit", MapType());
        } else {
            log(WARNING, "Got clothing with non-string worn attribute.");
        }
        // FIXME Implement adding stuff to the outfit propert, as efficiently
        // as possible
        // Must make sure that we can install the entity we have already
        // looked up here, and fix the GuiseProperty code so it does not
        // need a repeat lookup
    } else {
        debug(std::cout << "Got wield for a tool" << std::endl << std::flush;);

        if (m_rightHandWieldConnection.connected()) {
            m_rightHandWieldConnection.disconnect();
        }

        // The value is ignored by the update handler, but should be the
        // right type.
        update_arg->setAttr("right_hand_wield", item->getId());
        // setAttr("right_hand_wield", item->getId());
        m_rightHandWield = EntityRef(item);

        m_rightHandWieldConnection = item->containered.connect(sigc::mem_fun(this, &Character::wieldDropped));

        debug(std::cout << "Wielding " << item->getId() << std::endl << std::flush;);
    }

    Update update;
    update->setTo(getId());
    update->setArgs1(update_arg);
    res.push_back(update);
}

void Character::AttackOperation(const Operation & op, OpVector & res)
{
    const std::string & from = op->getFrom();
    if (from == getId()) {
        return;
    }

    Entity * attack_ent = BaseWorld::instance().getEntity(op->getFrom());
    if (attack_ent == 0) {
        log(ERROR, "AttackOperation: Attack op from non-existant ID");
        return;
    }

    Character * attacker = dynamic_cast<Character *>(attack_ent);

    if (attacker == 0) {
        log(ERROR, "AttackOperation: Attack op from non-character entity");
        return;
    }

    if (attacker->m_task != 0) {
        log(ERROR, String::compose("AttackOperation: Attack op aborted "
                                   "because attacker %1(%2) busy.",
                                   attacker->getId(), attacker->getType()));
        return;
    }

    if (m_task != 0) {
        log(ERROR, String::compose("AttackOperation: Attack op aborted "
                                   "because defender %1(%2) busy.",
                                   getId(), getType()));
        return;
    }

    Task * combat = BaseWorld::instance().newTask("combat", *this);

    if (combat == 0) {
        log(ERROR, "Character::AttackOperation: Unable to create combat task");
        return;
    }

    setTask(combat);
    m_task->initTask(op, res);
    if (m_task->obsolete()) {
        clearTask();
        return;
    }

    combat = BaseWorld::instance().newTask("combat", *attacker);

    if (combat == 0) {
        log(ERROR, "Character::AttackOperation: Unable to create combat task");
        return;
    }

    attacker->setTask(combat);
    attacker->m_task->initTask(op, res);
    if (attacker->m_task->obsolete()) {
        attacker->clearTask();
        clearTask();
        return;
    }
}

/// \brief Filter a Login operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindLoginOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Logout operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindLogoutOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Add operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindAddOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Attack operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindAttackOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindAttackOperation: attack op has no argument");
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindAttackOperation: attack op arg has no ID");
        return;
    }
    const std::string & id = arg->getId();

    op->setTo(id);
    res.push_back(op);
}

/// \brief Filter a Setup operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindSetupOperation(const Operation & op, OpVector & res)
{
    Anonymous setup_arg;
    setup_arg->setName("mind");
    op->setArgs1(setup_arg);
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Use operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindUseOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Use op from mind" << std::endl << std::flush;);

    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        if (m_task != 0) {
            if (!m_task->obsolete()) {
                m_task->irrelevant();
            }
            assert(m_task->obsolete());
            clearTask();
        }
        return;
    }

    if (m_rightHandWield == 0) {
        error(op, "Character::mindUseOp No tool wielded.", res, getId());
        return;
    }
    // FIXME Get a tool id from the op attributes?

    Entity * tool = m_rightHandWield.get();
    if (tool == 0) {
        error(op, "Character::mindUseOp Tool does not exist.", res, getId());
        return;
    }

    Element toolOpAttr;
    std::set<std::string> toolOps;
    std::string op_type;

    // Determine the operations this tool supports
    if (!tool->getAttr("operations", toolOpAttr)) {
        log(NOTICE, "Character::mindUseOp Attempt to use something not a tool");
        return;
    }

    if (!toolOpAttr.isList()) {
        log(ERROR, "Character::mindUseOp Tool has non list operations list");
        return;
    }
    const ListType & toolOpList = toolOpAttr.asList();
    if (toolOpList.empty()) {
        log(ERROR, "Character::mindUseOp Tool operation list is empty");
        return;
    }
    ListType::const_iterator J = toolOpList.begin();
    ListType::const_iterator Jend = toolOpList.end();
    assert(J != Jend);
    if (!(*J).isString()) {
        log(ERROR, "Character::mindUseOp Tool operation list is malformed");
        return;
    }
    op_type = (*J).String();
    debug(std::cout << "default tool op is " << op_type << std::endl
                                                        << std::flush;);
    for (; J != Jend; ++J) {
        if (!(*J).isString()) {
            log(ERROR, "Character::mindUseOp Tool has non string in operations list");
        } else {
            toolOps.insert((*J).String());
        }
    }


    RootEntity entity_arg(0);

    assert(!entity_arg.isValid());

    // Look at Use args. If arg is an entity, this is the target.
    // If arg is an operation, this is the operation to be used, and the
    // sub op arg may be an entity specifying target. If op to be used is
    // specified, this is checked against the ops permitted by this tool.
    const Root & arg = args.front();
    const std::string & argtype = arg->getObjtype();
    if (argtype == "op") {
        if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG) ||
            (arg->getParents().empty())) {
            error(op, "Use arg op has malformed parents", res, getId());
            return;
        }
        op_type = arg->getParents().front();
        debug(std::cout << "Got op type " << op_type << " from arg"
                        << std::endl << std::flush;);
        if (toolOps.find(op_type) == toolOps.end()) {
            error(op, "Use op is not permitted by tool", res, getId());
            return;
        }
        // Check against valid ops
        Operation arg_op = smart_dynamic_cast<Operation>(arg);
        if (!arg_op.isValid()) {
            error(op, "Use op arg is a malformed op", res, getId());
            return;
        }

        const std::vector<Root> & arg_op_args = arg_op->getArgs();
        if (!arg_op_args.empty()) {
            entity_arg = smart_dynamic_cast<RootEntity>(arg_op_args.front());
            if (!entity_arg.isValid()) {
                error(op, "Use op target is malformed", res, getId());
                return;
            }
        }
    } else if (argtype == "obj") {
        entity_arg = smart_dynamic_cast<RootEntity>(arg);
        if (!entity_arg.isValid()) {
            error(op, "Use target is malformed", res, getId());
            return;
        }
    } else {
        error(op, "Use arg has unknown objtype", res, getId());
        return;
    }

    Anonymous target;
    if (!entity_arg.isValid()) {
        error(op, "Character::mindUseOperation No target specified", res, getId());
        return;
    }

    if (!entity_arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Character::mindUseOperation Target entity has no ID", res, getId());
        return;
    }
    target->setId(entity_arg->getId());

    if (entity_arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        target->setPos(entity_arg->getPos());
    }

    if (op_type.empty()) {
        error(op, "Character::mindUseOperation Unable to determine op type for tool", res, getId());
        return;
    }

    debug(std::cout << "Using tool " << tool->getType() << " on "
                    << target->getId()
                    << " with " << op_type << " action."
                    << std::endl << std::flush;);

    Root obj = Atlas::Objects::Factories::instance()->createObject(op_type);
    if (!obj.isValid()) {
        log(ERROR,
            String::compose("Character::mindUseOperation Unknown op type "
                            "\"%1\" requested by \"%2\" tool.",
                            op_type, tool->getType()));
        return;
    }
    Operation rop = smart_dynamic_cast<Operation>(obj);
    if (!rop.isValid()) {
        log(ERROR, String::compose("Character::mindUseOperation Op type "
                                   "\"%1\" requested by %2 tool, "
                                   "but it is not an operation type",
                                   op_type, tool->getType()));
        // FIXME Think hard about how this error is reported. Would the error
        // make it back to the client if we made an error response?
        return;
    } else if (!target->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        debug(std::cout << "No target" << std::endl << std::flush;);
    } else {
        rop->setArgs1(target);
    }

    rop->setTo(tool->getId());

    Entity * target_ent = BaseWorld::instance().getEntity(entity_arg->getId());
    if (target_ent == 0) {
        error(op, "Character::mindUseOperation Target does not exist", res, getId());
        return;
    }

    Task * task = BaseWorld::instance().activateTask(tool->getType()->name(),
                                                     op_type,
                                                     target_ent->getType()->name(),
                                                     *this);
    if (task != NULL) {
        setTask(task);
        assert(res.empty());
        m_task->initTask(rop, res);
        if (m_task->obsolete()) {
            clearTask();
        } else if (res.empty()) {
            // If initialising the task did not result in any operation at all
            // then the task cannot work correctly. In this case all we can
            // do is flag an error, and get rid of the task.
            log(ERROR, String::compose("Character::mindUseOperation Op type "
                                       "\"%1\" of tool \"%2\" activated a task,"
                                       " but it did not initialise",
                                       op_type, tool->getType()));
            m_task->irrelevant();
            clearTask();
        }
        return;
    }

    res.push_back(rop);

    Sight sight;
    sight->setArgs1(rop);
    res.push_back(sight);
}

/// \brief Filter a Update operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindUpdateOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Wield operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindWieldOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Wield op from mind" << std::endl << std::flush;);
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Tick operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindTickOperation(const Operation & op, OpVector & res)
{
    Anonymous tick_arg;
    tick_arg->setName("mind");
    op->setArgs1(tick_arg);
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Move operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindMoveOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mind_move_op" << std::endl << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindMoveOperation: move op has no argument");
        return;
    }
    const RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());
    if (!arg.isValid()) {
        log(ERROR, "mindMoveOperation: Arg is not an entity");
        return;
    }
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindMoveOperation: Arg has no ID");
        return;
    }
    if (getStamina() <= 0.f) {
        // Character is immobilised.
        return;
    }
    const std::string & other_id = arg->getId();
    if (other_id != getId()) {
        debug( std::cout << "Moving something else. " << other_id << std::endl << std::flush;);
        Entity * other = BaseWorld::instance().getEntity(other_id);
        if (other == 0) {
            Unseen u;

            Anonymous unseen_arg;
            unseen_arg->setId(other_id);
            u->setArgs1(unseen_arg);

            u->setTo(getId());
            res.push_back(u);
            return;
        }
        Element mass;
        if (!other->getAttr("mass", mass) || !mass.isFloat() ||
            mass.Float() > m_statistics.get("strength")) {
            debug( std::cout << "We can't move this. Just too heavy" << std::endl << std::flush;);
            return;
        }
        op->setTo(other_id);
        res.push_back(op);
        return;
    }
    std::string new_loc;
    if (arg->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        new_loc = arg->getLoc();
    } else {
        debug( std::cout << "Parent not set" << std::endl << std::flush;);
    }
    Point3D new_pos;
    Vector3D new_velocity;
    Quaternion new_orientation;
    try {
        if (arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
            fromStdVector(new_pos, arg->getPos());
            debug( std::cout << "pos set to " << new_pos << std::endl << std::flush;);
        }

        if (arg->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            fromStdVector(new_velocity, arg->getVelocity());
            debug( std::cout << "vel set to " << new_velocity
                             << std::endl << std::flush;);
        }

        Element orientation_attr;
        if (arg->copyAttr("orientation", orientation_attr) == 0) {
            new_orientation.fromAtlas(orientation_attr);
            debug( std::cout << "ori set to " << new_orientation << std::endl << std::flush;);
            if (!new_orientation.isValid()) {
                log(ERROR, "Invalid orientation from client. Ignoring");
            }
        }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Malformed move operation");
        return;
    }
    catch (...) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Unknown exception thrown");
        return;
    }

    debug( std::cout << ":" << new_loc << ":" << m_location.m_loc->getId()
                     << ":" << std::endl << std::flush;);
    if (!new_loc.empty() && (new_loc != m_location.m_loc->getId())) {
        debug(std::cout << "Changing loc" << std::endl << std::flush;);
        Entity * target_loc = BaseWorld::instance().getEntity(new_loc);
        if (target_loc == 0) {
            Unseen u;

            Anonymous unseen_arg;
            unseen_arg->setId(new_loc);
            u->setArgs1(unseen_arg);

            u->setTo(getId());
            res.push_back(u);
            return;
        }

        if (new_pos.isValid()) {
            Location target(target_loc, new_pos);
            Vector3D distance = distanceTo(m_location, target);
            assert(distance.isValid());
            // Convert target into our current frame of reference.
            new_pos = m_location.pos() + distance;
        } else {
            log(WARNING, "mindMoveOperation: Argument changes LOC, but no POS specified. Not sure this makes any sense");
        }
    }
    // Movement within current loc. Work out the speed and stuff and
    // use movement object to track movement.

    Location ret_location;
    int ret = m_movement.getUpdatedLocation(ret_location);
    if (ret) {
        ret_location = m_location;
    }

    // FIXME THis here?
    m_movement.reset();

    Vector3D direction;
    if (new_pos.isValid()) {
        direction = new_pos - ret_location.pos();
    } else if (new_velocity.isValid()) {
        direction = new_velocity;
    }
    if (direction.isValid() && (direction.mag() > 0)) {
        direction.normalize();
        debug( std::cout << "Direction: " << direction << std::endl
                         << std::flush;);
        if (!new_orientation.isValid()) {
            // This is a character walking, so it should stap upright
            Vector3D upright_direction = direction;
            upright_direction[cZ] = 0;
            if (upright_direction.mag() > 0) {
                upright_direction.normalize();
                new_orientation = quaternionFromTo(Vector3D(1,0,0),
                                                   upright_direction);
                debug( std::cout << "Orientation: " << new_orientation
                                 << std::endl << std::flush;);
            }
        }
    }

    double vel_mag;
    if (new_velocity.isValid()) {
        vel_mag = std::min(new_velocity.mag(), consts::base_velocity);
    } else {
        vel_mag = consts::base_velocity;
    }

    // Need to add the arguments to this op before we return it
    // direction is already a unit vector
    if (new_pos.isValid()) {
        m_movement.setTarget(new_pos);
        debug(std::cout << "Target" << new_pos
                        << std::endl << std::flush;);
    }
    if (direction.isValid()) {
        ret_location.m_velocity = direction;
        ret_location.m_velocity *= vel_mag;
        debug(std::cout << "Velocity" << ret_location.velocity()
                        << std::endl << std::flush;);
    }
    ret_location.m_orientation = new_orientation;
    debug(std::cout << "Orientation" << ret_location.orientation()
                    << std::endl << std::flush;);

    Operation move_op = m_movement.generateMove(ret_location);
    assert(move_op.isValid());
    res.push_back(move_op);

    if (m_movement.hasTarget() &&
        ret_location.velocity().isValid() &&
        ret_location.velocity() != Vector3D(0,0,0)) {

        Tick tickOp;
        Anonymous tick_arg;
        tick_arg->setAttr("serialno", m_movement.serialno());
        tick_arg->setName("move");
        tickOp->setArgs1(tick_arg);
        tickOp->setTo(getId());
        tickOp->setFutureSeconds(m_movement.getTickAddition(ret_location.pos(),
                                                     ret_location.velocity()));

        res.push_back(tickOp);
    }

}

/// \brief Filter a Set operation coming from the mind
///
/// Currently any Set op is permitted. In the future this will be locked
/// down to only allow mutable things to be changed. For example, for
/// inventory items with no name can have their name set from the client.
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindSetOperation(const Operation & op, OpVector & res)
{
    log(WARNING, "Set op from mind");
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindSetOperation: set op has no argument");
        return;
    }
    const Root & arg = args.front();
    if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        op->setTo(arg->getId());
    } else {
        op->setTo(getId());
    }
    res.push_back(op);
}

/// \brief Filter a Sight operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindSightOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Sound operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindSoundOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Combine operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindCombineOperation(const Operation & op, OpVector & res)
{
    std::cout << "mindCombineOperation" << std::endl << std::flush;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindCombineOperation: combine op has no argument");
        return;
    }
    std::vector<Root>::const_iterator I = args.begin();
    const Root & arg1 = *I;
    op->setTo(arg1->getId());
    std::vector<Root>::const_iterator Iend = args.end();
    for (; I != Iend; ++I) {
        const Root & arg = *I;
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Character::mindCombineOp No ID.", res, getId());
            return;
        }
        // FIXME Check item to be combined is in inventory
        // and then also check stackable and the same type.
    }
    res.push_back(op);
}

/// \brief Filter a Create operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindCreateOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Delete operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindDeleteOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Divide operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindDivideOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindDivideOperation: op has no argument");
        return;
    }
    std::vector<Root>::const_iterator I = args.begin();
    const Root & arg1 = *I;
    if (!arg1->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Character::mindDivideOp arg 1 has no ID.", res, getId());
        return;
    }
    // FIXME Check entity to be divided is in inventory
    op->setTo(arg1->getId());
    ++I;
    std::vector<Root>::const_iterator Iend = args.end();
    for (; I != Iend; ++I) {
        const Root & arg = *I;
        if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Character::mindDivideOp arg has ID.", res, getId());
            return;
        }
        // Check the same type?
    }
    res.push_back(op);
}

/// \brief Filter a Get operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindGetOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Imaginary operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindImaginaryOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Info operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindInfoOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Nourish operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindNourishOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Talk operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindTalkOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mindTalkOperation"
                     << std::endl << std::flush;);
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Look operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindLookOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got look up from mind from [" << op->getFrom()
               << "] to [" << op->getTo() << "]" << std::endl << std::flush;);
    m_perceptive = true;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        op->setTo(BaseWorld::instance().m_gameWorld.getId());
    } else {
        const Root & arg = args.front();
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            log(ERROR, "mindLookOperation: Op has no ID");
            return;
        }
        op->setTo(arg->getId());
    }
    debug( std::cout <<"  now to ["<<op->getTo()<<"]"<<std::endl<<std::flush;);
    res.push_back(op);
}

/// \brief Filter a Eat operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindEatOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindEatOperation: Op has no ARGS");
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindEatOperation: Arg has no ID");
        return;
    }
    op->setTo(arg->getId());
    res.push_back(op);
}

/// \brief Filter a Touch operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindTouchOperation(const Operation & op, OpVector & res)
{
    // Work out what is being touched.
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindTouchOperation: Op has no ARGS");
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindTouchOperation: Op has no ID");
        return;
    }
    // Pass the modified touch operation on to target.
    op->setTo(arg->getId());
    res.push_back(op);
    // Send sight of touch
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
}

/// \brief Filter a Appearance operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindAppearanceOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter a Disappearance operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindDisappearanceOperation(const Operation & op, OpVector & res)
{
}


/// \brief Filter a Error operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindErrorOperation(const Operation & op, OpVector & res)
{
}

/// \brief Filter any other operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindOtherOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Appearance operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mAppearanceOperation(const Operation & op)
{
    return true;
}

/// \brief Filter a Disappearance operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mDisappearanceOperation(const Operation & op)
{
    return true;
}

/// \brief Filter a Error operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mErrorOperation(const Operation & op)
{
    return true;
}

/// \brief Filter a Setup operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mSetupOperation(const Operation & op)
{
    if (!op->getArgs().empty()) {
        if (op->getArgs().front()->getName() == "mind") {
            return true;
        }
    }
    return false;
}

/// \brief Filter a Tick operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mTickOperation(const Operation & op)
{
    if (!op->getArgs().empty()) {
        if (op->getArgs().front()->getName() == "mind") {
            return true;
        }
    }
    return false;
}

/// \brief Filter a Unseen operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mUnseenOperation(const Operation & op)
{
    return true;
}

/// \brief Filter a Sight operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mSightOperation(const Operation & op)
{
    return true;
}

/// \brief Filter a Sound operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mSoundOperation(const Operation & op)
{
    return true;
}

/// \brief Filter a Touch operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mTouchOperation(const Operation & op)
{
    return true;
}

/// \brief Send an operation to the current active part of the mind
///
/// The operation can potentially go to an external mind if one is
/// currently one attached to this Character. This is normally a player
/// client, but could be a remote AI agent. Additionally the operation
/// can go to an internal AI mind. The result from the AI mind is
/// discarded if an external mind is connected.
/// @param op Operation to be processed.
/// @param res The result of the operation is returned here.
void Character::sendMind(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::sendMind(" << op->getParents().front() << ")" << std::endl << std::flush;);

    if (0 != m_externalMind) {
        if (0 != m_mind) {
            OpVector mindRes;
            m_mind->operation(op, mindRes);
            // Discard all the local results
        }
        debug(std::cout << "Sending to external mind" << std::endl
                         << std::flush;);
        m_externalMind->operation(op, res);
    } else {
        debug(std::cout << "Using ops from local mind"
                        << std::endl << std::flush;);
        if (0 != m_mind) {
            m_mind->operation(op, res);
        }
    }
}

/// \brief Filter operations from the mind destined for the body.
///
/// Operations from the character's mind which is either an NPC mind,
/// or a remote client are passed in here for pre-processing and filtering
/// before they are valid to be processed as internal ops. The operation
/// may be modified and re-used so operations passed to this function have
/// their ownership passed in, and caller should not modify the operation,
/// make assumptions that it has not been modified after calling mind2body.
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Character::mind2body(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mind2body(" << std::endl << std::flush;);

    if (!op->isDefaultTo()) {
        log(ERROR, String::compose("Operation \"%1\" from mind with TO set.",
                                   op->getParents().front()));
        return;
    }
    if (!op->isDefaultFutureSeconds() && op->getClassNo() != OP_TICK) {
        log(ERROR, String::compose("Operation \"%1\" from mind with "
                                   "FUTURE_SECONDS set.",
                                   op->getParents().front()));
    }
    OpNo otype = op->getClassNo();
    OP_SWITCH(op, otype, res, mind)
}

/// \brief Filter operations from the world to the mind
///
/// Operations from the world are checked here to see if they are suitable
/// to send to the mind. Some operations should not go to the mind as they
/// leak information. Others are just not necessary as they provide no
/// useful information.
bool Character::world2mind(const Operation & op)
{
    debug( std::cout << "Character::world2mind(" << op->getParents().front() << ")" << std::endl << std::flush;);
    OpNo otype = op->getClassNo();
    POLL_OP_SWITCH(op, otype, w2m)
    return false;
}

void Character::operation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::operation(" << op->getParents().front() << ")" << std::endl << std::flush;);
    Entity::operation(op, res);
    // set refno on result?
    if (!m_isAlive) {
        return;
    }
    if (world2mind(op)) {
        debug( std::cout << "Character::operation(" << op->getParents().front() << ") passed to mind" << std::endl << std::flush;);
        OpVector mres;
        sendMind(op, mres);
        OpVector::const_iterator Iend = mres.end();
        for (OpVector::const_iterator I = mres.begin(); I != Iend; ++I) {
            externalOperation(*I);
        }
    }
}

void Character::externalOperation(const Operation & op)
{
    debug( std::cout << "Character::externalOperation(" << op->getParents().front() << ")" << std::endl << std::flush;);
    OpVector mres;
    mind2body(op, mres);
    
    OpVector::const_iterator I = mres.begin();
    OpVector::const_iterator Iend = mres.end();

    // If the original op had a serial no, we assume the first consequence
    // of that is effectively the same operation.
    // FIXME Can this be guaranteed by the mind2body phase?
    if (!op->isDefaultSerialno() && I != Iend) {
        (*I)->setSerialno(op->getSerialno());
    }

    for (; I != Iend; ++I) {
        sendWorld(*I);
    }
}
