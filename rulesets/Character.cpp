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


#include "Character.h"

#include "Pedestrian.h"
#include "ProxyMind.h"
#include "EntityProperty.h"
#include "ExternalMind.h"
#include "ExternalProperty.h"
#include "OutfitProperty.h"
#include "StatusProperty.h"
#include "TasksProperty.h"

#include "common/BaseWorld.h"
#include "common/op_switch.h"
#include "common/const.h"
#include "common/custom.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/Link.h"
#include "common/TypeNode.h"
#include "common/serialno.h"
#include "common/compose.hpp"
#include "common/PropertyManager.h"

#include "common/Actuate.h"
#include "common/Attack.h"
#include "common/Eat.h"
#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"
#include "common/Update.h"
#include "common/Think.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/hide.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

long int Character::s_serialNumberNext = 0L;

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

static const std::string FOOD = "food";
static const std::string MASS = "mass";
static const std::string MAXMASS = "maxmass";
static const std::string OUTFIT = "outfit";
static const std::string RIGHT_HAND_WIELD = "right_hand_wield";
static const std::string SERIALNO = "serialno";
static const std::string STAMINA = "stamina";
static const std::string STATUS = "status";
static const std::string TASKS = "tasks";

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
    // Currently handles energy
    // We should probably call this whenever the entity performs a movement.

    StatusProperty * status_prop = modPropertyClass<StatusProperty>(STATUS);
    bool status_changed = false;
    if (status_prop == 0) {
        // FIXME Probably don't do enough here to set up the property.
        status_prop = new StatusProperty;
        assert(status_prop != 0);
        m_properties[STATUS] = status_prop;
        status_prop->set(1.f);
        status_changed = true;
    }
    double & status = status_prop->data();
    status_prop->setFlags(flag_unsent);

    Property<double> * food_prop = modPropertyType<double>(FOOD);
    // DIGEST
    if (food_prop != 0) {
        double & food = food_prop->data();
        if (food >= foodConsumption && status < 2) {
            // It is important that the metabolise bit is done next, as this
            // handles the status change
            status += foodConsumption;
            status_changed = true;
            food -= foodConsumption;

            food_prop->setFlags(flag_unsent);
            food_prop->apply(this);
        }
    }

    Property<double> * mass_prop = modPropertyType<double>(MASS);
    // If status is very high, we gain weight
    if (status > (1.5 + energyLaidDown)) {
        status -= energyLaidDown;
        status_changed = true;
        if (mass_prop != 0) {
            double & mass = mass_prop->data();
            mass += weightGain;
            mass_prop->setFlags(flag_unsent);
            Element maxmass_attr;
            if (getAttrType(MAXMASS, maxmass_attr, Element::TYPE_FLOAT) == 0) {
                mass = std::min(mass, maxmass_attr.Float());
            }
            mass_prop->apply(this);
        }
    } else {
        // If status is relatively is not very high, then energy is burned
        double energy_used = energyConsumption * ammount;
        status -= energy_used;
        status_changed = true;
        if (mass_prop != 0) {
            double & mass = mass_prop->data();
            double weight_used = weightConsumption * mass * ammount;
            if (status <= 0.5 && mass > weight_used) {
                // Drain away a little less energy and lose some weight
                // This ensures there is a long term penalty to allowing
                // something to starve
                status += (energy_used / 2);
                status_changed = true;
                mass -= weight_used;
                mass_prop->setFlags(flag_unsent);
                mass_prop->apply(this);
            }
        }
    }
    // FIXME Stamina property?
    const TasksProperty * tp = getPropertyClass<TasksProperty>(TASKS);
    if ((tp == 0 || !tp->busy()) && !m_movement.updateNeeded(m_location)) {

        Property<double> * stamina_prop = modPropertyType<double>(STAMINA);
        if (stamina_prop != 0) {
            double & stamina = stamina_prop->data();
            if (stamina < 1.f) {
                stamina = 1.f;
                stamina_prop->setFlags(flag_unsent);
                stamina_prop->apply(this);
            }
        }
    }

    if (status_changed) {
        status_prop->apply(this);
    }

    Update update;
    update->setTo(getId());

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
    if (ent->m_contains == 0) {
        return 0;
    }
    LocatedEntitySet::const_iterator I = ent->m_contains->begin();
    LocatedEntitySet::const_iterator Iend = ent->m_contains->end();
    for (; I != Iend; ++I) {
        LocatedEntity * child = *I;
        if (child->getId() == id) {
            return *I;
        }
        if (child->m_contains != 0 && !child->m_contains->empty()) {
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
           Thing(id, intId),
               m_movement(*new Pedestrian(*this)),
               m_proxyMind(new ProxyMind(id, intId, *this)), m_externalMind(0)
{
    //Prevent the proxy mind from being deleted when all references to itself are removed
    //(for example through a Sight of a Delete).
    m_proxyMind->incRef();
    // FIXME Do we still need this?
    // It is my hope that once the task object is fully held by the
    // property, this will no longer be necessary. If it is we will
    // need to find another way.
    // destroyed.connect(sigc::mem_fun(this, &Character::clearTask));
}

Character::~Character()
{
    if (m_rightHandWieldConnection.connected()) {
        m_rightHandWieldConnection.disconnect();
    }
    delete &m_movement;
    delete m_proxyMind;
    delete m_externalMind;
}

int Character::linkExternal(Link * link)
{
    if (m_externalMind == 0) {
        m_externalMind = new ExternalMind(*this);
    } else if (m_externalMind->isLinked()) {
        return -1;
    }
    m_externalMind->linkUp(link);

    if (getProperty("external") == 0) {
        ExternalProperty * ep = new ExternalProperty(m_externalMind);
        // FIXME ensure this is install()ed and apply()ed
        setProperty("external", ep);
    }

    Anonymous update_arg;
    update_arg->setId(getId());
    update_arg->setAttr("external", 1);

    Update update;
    update->setTo(getId());
    update->setArgs1(update_arg);

    sendWorld(update);

    //Now that we're connected we need to send any thoughts that we've been given to the mind client.
    auto thoughts = m_proxyMind->getThoughts();
    //We need to clear the existing thoughts since we'll be sending them anew; else we'll end up with duplicates.
    m_proxyMind->clearThoughts();
    Atlas::Objects::Operation::Think think;
    Atlas::Objects::Operation::Set setThoughts;
    setThoughts->setArgs(thoughts);
    think->setArgs1(setThoughts);
    think->setTo(getId());
    sendWorld(think);

    externalLinkChanged.emit();
    return 0;
}

int Character::unlinkExternal(Link * link)
{
    if (m_externalMind == 0) {
        log(ERROR, "Character is not connected. " + describeEntity());
        return -1;
    }

    if  (!m_externalMind->isLinkedTo(link)) {
        if (m_externalMind->isLinked()) {
            return -2;
        }
        return -1;
    }

    // Send a move op stopping the current movement
    Anonymous move_arg;
    move_arg->setId(getId());
    // Include the EXTERNAL property which is changing to zero.
    // It would be more correct at this point to send a separate
    // update to have the property update itself, but this
    // will be much less of an issue once Sight(Set) is created
    // more correctly
    move_arg->setAttr("external", 0);
    ::addToEntity(Vector3D(0,0,0), move_arg->modifyVelocity());

    Move move;
    move->setFrom(getId());
    move->setArgs1(move_arg);

    filterExternalOperation(move);


    // We used to delete the external mind here, but now we
    // leave it in place, as it takes care of the disconnected
    // character.
    m_externalMind->linkUp(0);
    externalLinkChanged.emit();
    return 0;
}

/// \brief Set up a new task as the one being performed by the Character
///
/// @param task The new task to be assigned to the Character
/// @param op The operation that initiates the task.
/// @param res The result of the task startup.
int Character::startTask(Task * task, const Operation & op, OpVector & res)
{
    TasksProperty * tp = requirePropertyClass<TasksProperty>(TASKS);

    return tp->startTask(task, this, op, res);
}

/// \brief Update the visible representation of the current task
///
/// Generate a Set operation which modifies the Characters task property
/// to reflect the current status of the task.
void Character::updateTask(OpVector & res)
{
    TasksProperty * tp = requirePropertyClass<TasksProperty>(TASKS);

    tp->updateTask(this, res);
}

/// \brief Clean up and remove the task currently being executed
///
/// Remove the task, and send an operation indicating that no tasks
/// are now present.
void Character::clearTask(OpVector & res)
{
    TasksProperty * tp = modPropertyClass<TasksProperty>(TASKS);

    if (tp == 0) {
        log(NOTICE, "Clearing task when no property exists. " + describeEntity());
        return;
    }

    tp->clearTask(this, res);
}

std::vector<Atlas::Objects::Root> Character::getThoughts() const
{
    if (m_proxyMind) {
        return m_proxyMind->getThoughts();
    }
    return LocatedEntity::getThoughts();
}


void Character::ImaginaryOperation(const Operation & op, OpVector & res)
{
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
}

void Character::InfoOperation(const Operation & op, OpVector & res)
{
    TasksProperty * tp = modPropertyClass<TasksProperty>(TASKS);

    if (tp == 0) {
        return;
    }

    tp->operation(this, op, res);
}

void Character::TickOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "================================" << std::endl
                    << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (!args.empty()) {
        const Root & arg = args.front();
        if (arg->getName() == "move") {
//            // Deal with movement.
//            Element serialno;
//            if (arg->copyAttr(SERIALNO, serialno) == 0 && (serialno.isInt())) {
//                if (serialno.asInt() < m_movement.serialno()) {
//                    debug(std::cout << "Old tick" << std::endl << std::flush;);
//                    return;
//                }
//            } else {
//                log(ERROR, "Character::TickOperation: No serialno in tick arg");
//            }
//            Location return_location;
//            if (m_movement.getUpdatedLocation(return_location)) {
//                return;
//            }
//            res.push_back(m_movement.generateMove(return_location));
//            Anonymous tick_arg;
//            tick_arg->setName("move");
//            tick_arg->setAttr(SERIALNO, m_movement.serialno());
//            Tick tickOp;
//            tickOp->setTo(getId());
//            tickOp->setFutureSeconds(m_movement.getTickAddition(return_location.pos(), return_location.velocity()));
//            tickOp->setArgs1(tick_arg);
//            res.push_back(tickOp);
        } else if (arg->getName() == "task") {
            TasksProperty * tp = modPropertyClass<TasksProperty>(TASKS);

            if (tp == 0) {
                log(ERROR, "Tick for task, but not tasks property. " + describeEntity());
                return;
            }

            tp->TickOperation(this, op, res);

        } else if (arg->getName() == "mind") {
            // Do nothing. Passed to mind.
        } else {
            debug(std::cout << "Tick to unknown subsystem " << arg->getName()
                            << " arrived. " << describeEntity() << std::endl << std::flush
            ;);
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
    debug(std::cout << "Character::Operation(Talk) " << describeEntity() << std::endl << std::flush
    ;);
    Sound s;
    s->setArgs1(op);
    res.push_back(s);
}

void Character::NourishOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        error(op, "Nourish has no argument.", res, getId());
        return;
    }
    const Root & arg = op->getArgs().front();
    Element mass_attr;
    if (arg->copyAttr(MASS, mass_attr) != 0 || !mass_attr.isNum()) {
        return;
    }

    Property<double> * food_prop = requirePropertyClass<Property<double> >(FOOD, 0.f);
    double & food = food_prop->data();
    food += mass_attr.asNum();
    food_prop->setFlags(flag_unsent);

    // FIXME This will become a Update once private properties are sorted
    Anonymous food_ent;
    food_ent->setId(getId());
    food_ent->setAttr(FOOD, food);

    Set s;
    s->setArgs1(food_ent);
    // FIXME FROM, SECONDS?

    Sight si;
    si->setTo(getId());
    si->setArgs1(s);
    res.push_back(si);
}

void Character::UseOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Use op. " << describeEntity() << std::endl << std::flush
    ;);

    TasksProperty * tp = modPropertyClass<TasksProperty>(TASKS);

    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        if (tp != 0) {
            tp->stopTask(this, res);
        }
        return;
    }

    // Are we going to modify this really?
    EntityProperty * rhw = modPropertyClass<EntityProperty>(RIGHT_HAND_WIELD);
    if (rhw == 0) {
        error(op, "Character::UseOp No tool wielded, no right_hand_wield property found", res, getId());
        return;
    }

    LocatedEntity * tool = rhw->data().get();
    if (tool == 0) {
        error(op, "Character::UseOp No tool wielded, no entity found", res, getId());
        return;
    }
    // FIXME Get a tool id from the op attributes?

    Element toolOpAttr;
    std::set<std::string> toolOps;
    std::string op_type;

    // Determine the operations this tool supports
    if (tool->getAttr("operations", toolOpAttr) != 0) {
        log(NOTICE, "Character::UseOp Attempt to use something not a tool. " + describeEntity());
        return;
    }

    if (!toolOpAttr.isList()) {
        log(ERROR, "Character::UseOp Tool has non list operations list. " + describeEntity());
        return;
    }
    const ListType & toolOpList = toolOpAttr.asList();
    if (toolOpList.empty()) {
        log(ERROR, "Character::UseOp Tool operation list is empty. " + describeEntity());
        return;
    }
    ListType::const_iterator J = toolOpList.begin();
    ListType::const_iterator Jend = toolOpList.end();
    assert(J != Jend);
    if (!(*J).isString()) {
        log(ERROR, "Character::UseOp Tool operation list is malformed. " + describeEntity());
        return;
    }
    op_type = (*J).String();
    debug(std::cout << "default tool op is " << op_type << std::endl << std::flush
    ;);
    for (; J != Jend; ++J) {
        if (!(*J).isString()) {
            log(ERROR, "Character::UseOp Tool has non string in operations list." + describeEntity());
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
        if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG) || (arg->getParents().empty())) {
            error(op, "Use arg op has malformed parents", res, getId());
            return;
        }
        op_type = arg->getParents().front();
        debug(std::cout << "Got op type " << op_type << " from arg" << std::endl << std::flush
        ;);
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
        error(op, "Character::UseOperation No target specified", res, getId());
        return;
    }

    if (!entity_arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Character::UseOperation Target entity has no ID", res, getId());
        return;
    }
    target->setId(entity_arg->getId());

    if (entity_arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        target->setPos(entity_arg->getPos());
    }

    if (op_type.empty()) {
        error(op, "Character::UseOperation Unable to determine op type for tool", res, getId());
        return;
    }

    debug(std::cout << "Using tool " << tool->getType() << " on " << target->getId() << " with " << op_type << " action." << std::endl << std::flush
    ;);

    Root obj = Atlas::Objects::Factories::instance()->createObject(op_type);
    if (!obj.isValid()) {
        log(ERROR, String::compose("Character::UseOperation Unknown op type "
                "\"%1\" requested by \"%2\" tool. %3", op_type, tool->getType(), describeEntity()));
        return;
    }
    Operation rop = smart_dynamic_cast<Operation>(obj);
    if (!rop.isValid()) {
        log(ERROR, String::compose("Character::UseOperation Op type "
                "\"%1\" requested by %2 tool, "
                "but it is not an operation type. %3", op_type, tool->getType(), describeEntity()));
        // FIXME Think hard about how this error is reported. Would the error
        // make it back to the client if we made an error response?
        return;
    } else if (!target->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        debug(std::cout << "No target" << std::endl << std::flush
        ;);
    } else {
        rop->setArgs1(target);
    }

    rop->setTo(tool->getId());

    LocatedEntity * target_ent = BaseWorld::instance().getEntity(entity_arg->getId());
    if (target_ent == 0) {
        error(op, "Character::UseOperation Target does not exist", res, getId());
        return;
    }

    Task * task = BaseWorld::instance().activateTask(tool->getType()->name(), op_type, target_ent, *this);
    if (task != NULL) {
        startTask(task, rop, res);
    }

    res.push_back(rop);

    Sight sight;
    sight->setArgs1(rop);
    res.push_back(sight);
}

void Character::WieldOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        EntityProperty * rhw = modPropertyClass<EntityProperty>(RIGHT_HAND_WIELD);
        if (rhw == 0) {
            return;
        }

        rhw->data() = EntityRef(0);
        rhw->setFlags(flag_unsent);
        // FIXME Remove the property?

        // FIXME Make sure we stop wielding if the container changes,
        // but connections are cleared, and don't build up.
        if (m_rightHandWieldConnection.connected()) {
            m_rightHandWieldConnection.disconnect();
        }

        Update update;
        update->setTo(getId());
        res.push_back(update);

        return;
    }
    const Root & arg = op->getArgs().front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Wield arg has no ID", res, getId());
        return;
    }
    const std::string & id = arg->getId();
    LocatedEntity * item = BaseWorld::instance().getEntity(id);
    if (item == 0) {
        error(op, "Wield arg does not exist", res, getId());
        return;
    }

    if (m_contains == 0 || m_contains->find(item) == m_contains->end()) {
        error(op, String::compose("Wield arg %1(%2) is not in inventory "
                "of %3(%4)", item->getType()->name(), id, getType()->name(), getId()), res, getId());
        return;
    }

    Element worn_attr;
    if (item->getAttr("worn", worn_attr) == 0) {
        debug(std::cout << "Got wield for a garment" << std::endl << std::flush
        ;);

        if (worn_attr.isString()) {
            OutfitProperty * outfit = requirePropertyClass<OutfitProperty>(OUTFIT);
            outfit->wear(this, worn_attr.String(), item);
            outfit->cleanUp();

            outfit->setFlags(flag_unsent);
        } else {
            log(WARNING, "Got clothing with non-string worn attribute. " + describeEntity());
            return;
        }
        // FIXME Implement adding stuff to the outfit propert, as efficiently
        // as possible
        // Must make sure that we can install the entity we have already
        // looked up here, and fix the GuiseProperty code so it does not
        // need a repeat lookup
    } else {
        debug(std::cout << "Got wield for a tool" << std::endl << std::flush
        ;);

        EntityProperty * rhw = requirePropertyClass<EntityProperty>(RIGHT_HAND_WIELD);
        // FIXME Make sure we don't stay linked to the previous wielded
        // tool.
        if (m_rightHandWieldConnection.connected()) {
            m_rightHandWieldConnection.disconnect();
        }

        // The value is ignored by the update handler, but should be the
        // right type.
        rhw->data() = EntityRef(item);
        rhw->setFlags(flag_unsent);

        m_rightHandWieldConnection = item->containered.connect(sigc::hide<0>(sigc::mem_fun(this, &Character::wieldDropped)));

        debug(std::cout << "Wielding " << item->getId() << std::endl << std::flush
        ;);
    }

    Update update;
    update->setTo(getId());
    res.push_back(update);
}

void Character::AttackOperation(const Operation & op, OpVector & res)
{
    const std::string & from = op->getFrom();
    if (from == getId()) {
        return;
    }

    LocatedEntity * attack_ent = BaseWorld::instance().getEntity(op->getFrom());
    if (attack_ent == 0) {
        log(ERROR, "AttackOperation: Attack op from non-existant ID");
        return;
    }

    // FIXME Is this dynamic cast required?
    Character * attacker = dynamic_cast<Character *>(attack_ent);

    if (attacker == 0) {
        log(ERROR, "AttackOperation: Attack op from non-character entity");
        return;
    }

    const TasksProperty * atp = attacker->getPropertyClass<TasksProperty>(TASKS);
    if (atp != 0 && atp->busy()) {
        log(ERROR, String::compose("AttackOperation: Attack op aborted "
                "because attacker %1(%2) busy.", attacker->getId(), attacker->getType()));
        return;
    }

    TasksProperty * tp = requirePropertyClass<TasksProperty>(TASKS);
    if (tp != 0 && tp->busy()) {
        log(ERROR, String::compose("AttackOperation: Attack op aborted "
                "because defender %1(%2) busy.", getId(), getType()));
        return;
    }

    Task * combat = BaseWorld::instance().newTask("combat", *this);

    if (combat == 0) {
        log(ERROR, "Character::AttackOperation: Unable to create combat task");
        return;
    }

    if (tp->startTask(combat, this, op, res) != 0) {
        return;
    }

    combat = BaseWorld::instance().newTask("combat", *attacker);

    if (combat == 0) {
        log(ERROR, "Character::AttackOperation: Unable to create combat task");
        return;
    }

    if (attacker->startTask(combat, op, res) != 0) {
        clearTask(res);
        return;
    }
}

void Character::ActuateOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Actuate op" << std::endl << std::flush
    ;);

    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Character::mindActuateOp No arg.", res, getId());
        return;
    }

    RootEntity entity_arg(0);

    assert(!entity_arg.isValid());

    std::string op_type;

    // Look at Actuate args. If arg is an entity, this is the target.
    // If arg is an operation, this is the operation to be used, and the
    // sub op arg may be an entity specifying target. If op to be used is
    // specified, this is checked against the ops permitted by this tool.
    const Root & arg = args.front();
    const std::string & argtype = arg->getObjtype();
    if (argtype == "op") {
        if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG) || (arg->getParents().empty())) {
            error(op, "Use arg op has malformed parents", res, getId());
            return;
        }
        op_type = arg->getParents().front();
        debug(std::cout << "Got op type " << op_type << " from arg" << std::endl << std::flush
        ;);
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

    if (!entity_arg.isValid()) {
        error(op, "Character::mindActuateOperation No target specified", res, getId());
        return;
    }

    if (!entity_arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Character::mindActuateOperation Target entity has no ID", res, getId());
        return;
    }

    if (op_type.empty()) {
        error(op, "Character::mindActuateOperation Unable to determine op type for tool", res, getId());
        return;
    }

    LocatedEntity * device = BaseWorld::instance().getEntity(entity_arg->getId());

    Element deviceOpAttr;
    std::set<std::string> deviceOps;

    // Determine the actions this device supports
    if (device->getAttr("actions", deviceOpAttr) != 0) {
        log(NOTICE, "Character::mindActuateOp Attempt to actuate something not a device. " + describeEntity());
        return;
    }

    if (!deviceOpAttr.isList()) {
        log(ERROR, "Character::mindActuateOp device has non list operations list. " + describeEntity());
        return;
    }
    const ListType & deviceOpList = deviceOpAttr.asList();
    if (deviceOpList.empty()) {
        log(ERROR, "Character::mindActuateOp device operation list is empty. " + describeEntity());
        return;
    }
    ListType::const_iterator J = deviceOpList.begin();
    ListType::const_iterator Jend = deviceOpList.end();
    assert(J != Jend);

    for (; J != Jend; ++J) {
        if (!(*J).isString()) {
            log(ERROR, "Character::mindActuateOp device has non string in operations list. " + describeEntity());
        } else {
            deviceOps.insert((*J).String());
        }
    }

    if (op_type.empty()) {
        op_type = deviceOpList.front().asString();
    } else if (deviceOps.find(op_type) == deviceOps.end()) {
        error(op, "Actuate op is not permitted by device", res, getId());
        return;
    }

    debug(std::cout << "Actuating device " << device->getType() << " with " << op_type << " action." << std::endl << std::flush
    ;);

    Root obj = Atlas::Objects::Factories::instance()->createObject(op_type);
    if (!obj.isValid()) {
        log(ERROR, String::compose("Character::mindActuateOperation Unknown op type "
                "\"%1\" requested by \"%2\" device. %3", op_type, device->getType(), describeEntity()));
        return;
    }
    Operation rop = smart_dynamic_cast<Operation>(obj);
    if (!rop.isValid()) {
        log(ERROR, String::compose("Character::mindActuateOperation Op type "
                "\"%1\" requested by %2 device, "
                "but it is not an operation type. %3", op_type, device->getType(), describeEntity()));
        // FIXME Think hard about how this error is reported. Would the error
        // make it back to the client if we made an error response?
        return;
    }

    rop->setTo(device->getId());

    res.push_back(rop);

    // Sight sight;
    // sight->setArgs1(rop);
    // res.push_back(sight);
}

void Character::RelayOperation(const Operation & op, OpVector & res)
{
    //A Relay operation with refno sent to ourselves signals that we should prune
    //our registered relays in m_relays. This is a feature to allow for a timeout; if
    //no Relay has been received from the destination Entity after a certain period
    //we'll shut down the relay link.
    if (op->getTo() == getId() && op->getFrom() == getId() && !op->isDefaultRefno()) {
        auto I = m_relays.find(op->getRefno());
        if (I != m_relays.end()) {
            //Also send a no-op to any client to make it stop waiting for any response.
            Operation noop;
            noop->setRefno(I->second.serialno);
            noop->setTo(getId());
            noop->setFrom(getId());
            OpVector mres;
            sendMind(noop, mres);
            m_relays.erase(I);
            for (auto& resOp : mres) {
                filterExternalOperation(resOp);
            }
        }
    } else {
        if (op->getArgs().empty()) {
            log(ERROR, "Character::RelayOperation no args. " + describeEntity());
            return;
        }
        Operation relayedOp = Atlas::Objects::smart_dynamic_cast<Operation>(op->getArgs().front());

        if (!relayedOp.isValid()) {
            log(ERROR, "Character::RelayOperation first arg is not an operation. " + describeEntity());
            return;
        }

        //If a relay op has a refno, it's a response to a Relay op previously sent out to another
        //entity, and we should send the incoming relayed operation to the mind.
        if (!op->isDefaultRefno()) {
            //Note that the relayed op should be considered untrusted in this case, as it has originated
            //from a random entity or its mind.
            auto I = m_relays.find(op->getRefno());
            if (I == m_relays.end()) {
                log(WARNING, "Character::RelayOperation could not find registrered Relay with refno. " + describeEntity());
                return;
            }

            //Make sure that this op really comes from the entity the original Relay op was sent to.
            if (op->getFrom() != I->second.destination) {
                log(WARNING, "Character::RelayOperation got relay op with mismatching 'from'. " + describeEntity());
                return;
            }

            //Get the relayed operation and send to mind.
            //Note that we don't send the operation to the entity; this is because we have
            //to treat the relayed operation as "unsafe". This is since it originated from an random
            //entity's mind and could in effect be anything (Set, Logout etc.)
            //We should therefore handle it with care and only send it on to the mind.
            //This of course hinges on the mind client code making sure to handle it correctly, given
            //its refno.
            relayedOp->setRefno(I->second.serialno);
            OpVector mres;
            //We only send to external minds; never to internal minds or proxy minds.
            if (m_externalMind) {
                m_externalMind->operation(relayedOp, mres);
            }
            m_relays.erase(I);
            for (auto& resOp : mres) {
                filterExternalOperation(resOp);
            }
        } else {

            //Check if the mind should handle the relayed operation; else we'll just let the
            //standard Entity relay code do it's thing.
            if (!world2mind(relayedOp)) {
                //This operation won't be sent to the mind, we'll pass it on to the standard
                //relay method which will generate a Sight as response.
                Entity::RelayOperation(op, res);
                return;
            }

            //If the Relay op instead has a serial no, it's a Relay op sent from us by another Entity
            //which expects a response. We should send it on to the mind (efter registering an entry in
            //m_relays to be handled by mind2body).
            //Note that the relayed operation in this case should be considered "trusted", as it has originated
            //from either the server itself or a trusted client.

            //Extract the contained operation, and register the relay into m_relays
            if (op->isDefaultSerialno()) {
                log(ERROR, "Character::RelayOperation no serial number. " + describeEntity());
                return;
            }

            Relay relay;
            relay.serialno = op->getSerialno();
            relay.destination = op->getFrom();

            //Generate a local serial number which we'll register in m_relays. When a response is received
            //we'll check the refno and match it against what we've stored
            long int serialNo = ++s_serialNumberNext;
            relayedOp->setSerialno(serialNo);
            m_relays.insert(std::make_pair(serialNo, relay));

            //Make sure that the contained op is addressed to the entity
            relayedOp->setTo(getId());

            //Now send the contained op to the entity
            operation(relayedOp, res);

            //Also send a future Relay op to ourselves to make sure that the registered relay in m_relays
            //is removed in the case that we don't get any response.
            Atlas::Objects::Operation::Generic pruneOp;
            pruneOp->setType("relay", Atlas::Objects::Operation::RELAY_NO);
            pruneOp->setTo(getId());
            pruneOp->setFrom(getId());
            pruneOp->setRefno(serialNo);
            //5 seconds should be more than enough.
            pruneOp->setFutureSeconds(5);
            sendWorld(pruneOp);
        }
    }
}

/// \brief Filter an Actuate operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindActuateOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Actuate op from mind" << std::endl << std::flush
    ;);

    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Attack operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindAttackOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindAttackOperation: attack op has no argument. " + describeEntity());
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindAttackOperation: attack op arg has no ID. " + describeEntity());
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
    debug(std::cout << "Got Use op from mind" << std::endl << std::flush
    ;);
    op->setTo(getId());
    res.push_back(op);
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
    debug(std::cout << "Got Wield op from mind" << std::endl << std::flush
    ;);
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
    debug(std::cout << "Character::mind_move_op" << std::endl << std::flush
    ;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindMoveOperation: move op has no argument. " + describeEntity());
        return;
    }
    const RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());
    if (!arg.isValid()) {
        log(ERROR, "mindMoveOperation: Arg is not an entity. " + describeEntity());
        return;
    }
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindMoveOperation: Arg has no ID. " + describeEntity());
        return;
    }
    Element stamina_attr;
    if (getAttrType(STAMINA, stamina_attr, Element::TYPE_FLOAT) == 0) {
        if (stamina_attr.Float() <= 0.f) {
            // Character is immobilised.
            return;
        }
    }
    const std::string & other_id = arg->getId();
    if (other_id != getId()) {
        debug(std::cout << "Moving something else. " << other_id << std::endl << std::flush
        ;);
        LocatedEntity * other = BaseWorld::instance().getEntity(other_id);
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
        if (other->getAttr(MASS, mass) != 0 || !mass.isFloat()) {
            // FIXME Check against strength
            // || mass.Float() > m_statistics.get("strength"));
            debug(std::cout << "We can't move this. Just too heavy" << std::endl << std::flush
            ;);
            //TODO: send op back to the mind informing it that it was too heavy to move.
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
        debug(std::cout << "Parent not set" << std::endl << std::flush
        ;);
    }
    Point3D new_pos;
    Vector3D new_velocity;
    Quaternion new_orientation;
    try {
        if (arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
            fromStdVector(new_pos, arg->getPos());
            debug(std::cout << "pos set to " << new_pos << std::endl << std::flush
            ;);
        }

        if (arg->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            fromStdVector(new_velocity, arg->getVelocity());
            debug(std::cout << "vel set to " << new_velocity << std::endl << std::flush
            ;);
        }

        Element orientation_attr;
        if (arg->copyAttr("orientation", orientation_attr) == 0) {
            new_orientation.fromAtlas(orientation_attr);
            debug(std::cout << "ori set to " << new_orientation << std::endl << std::flush
            ;);
            if (!new_orientation.isValid()) {
                log(ERROR, "Invalid orientation from client. Ignoring");
            }
        }
    } catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Malformed move operation. " + describeEntity());
        return;
    } catch (...) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Unknown exception thrown. " + describeEntity());
        return;
    }

    debug(std::cout << ":" << new_loc << ":" << m_location.m_loc->getId() << ":" << std::endl << std::flush
    ;);
    if (!new_loc.empty() && (new_loc != m_location.m_loc->getId())) {
        debug(std::cout << "Changing loc" << std::endl << std::flush
        ;);
        LocatedEntity * target_loc = BaseWorld::instance().getEntity(new_loc);
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
            log(WARNING, "mindMoveOperation: Argument changes LOC, but no POS specified. Not sure this makes any sense. " + describeEntity());
        }
    }
    // Movement within current loc. Work out the speed and stuff and
    // use movement object to track movement.

    Location ret_location = m_location;
//    int ret = m_movement.getUpdatedLocation(ret_location);
//    if (ret) {
//        ret_location = m_location;
//    }
//
//    // FIXME THis here?
//    m_movement.reset();

    Vector3D direction;
    if (new_pos.isValid()) {
        direction = new_pos - ret_location.pos();
    } else if (new_velocity.isValid()) {
        direction = new_velocity;
    }
    if (direction.isValid() && (direction.mag() > 0)) {
        direction.normalize();
        debug_print("Direction: " << direction);
        if (!new_orientation.isValid()) {
            // This is a character walking, so it should stay upright
            Vector3D upright_direction = direction;
            upright_direction[cZ] = 0;
            if (upright_direction.mag() > 0) {
                upright_direction.normalize();
                new_orientation = quaternionFromTo(Vector3D(1, 0, 0), upright_direction, Vector3D(0, 0, 1));
                debug_print("Orientation: " << new_orientation);
            }
        }
    }

    WFMath::CoordType vel_mag;
    if (new_velocity.isValid()) {
        vel_mag = std::min(new_velocity.mag(), consts::base_velocity);
    } else {
        vel_mag = consts::base_velocity;
    }

    // Need to add the arguments to this op before we return it
    // direction is already a unit vector
    if (new_pos.isValid()) {
        m_movement.setTarget(new_pos);
        debug(std::cout << "Target" << new_pos << std::endl << std::flush
        ;);
    }
    if (direction.isValid()) {
        ret_location.m_velocity = direction;
        ret_location.m_velocity *= vel_mag;
        debug(std::cout << "Velocity" << ret_location.velocity() << std::endl << std::flush
        ;);
    }
    ret_location.m_orientation = new_orientation;
    debug(std::cout << "Orientation" << ret_location.orientation() << std::endl << std::flush
    ;);

    Operation move_op = m_movement.generateMove(ret_location);
    assert(move_op.isValid());
    res.push_back(move_op);

//    if (m_movement.hasTarget() && ret_location.velocity().isValid() && ret_location.velocity() != Vector3D::ZERO()) {
//
//        Tick tickOp;
//        Anonymous tick_arg;
//        tick_arg->setAttr(SERIALNO, m_movement.serialno());
//        tick_arg->setName("move");
//        tickOp->setArgs1(tick_arg);
//        tickOp->setTo(getId());
//        tickOp->setFutureSeconds(m_movement.getTickAddition(ret_location.pos(), ret_location.velocity()));
//
//        res.push_back(tickOp);
//    }

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
        log(ERROR, "mindSetOperation: set op has no argument. " + describeEntity());
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

/// \brief Filter a Combine operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindCombineOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "mindCombineOperation" << std::endl << std::flush
    ;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindCombineOperation: combine op has no argument. " + describeEntity());
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
        log(ERROR, "mindDivideOperation: op has no argument. " + describeEntity());
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

/// \brief Filter a Imaginary operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindImaginaryOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Talk operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindTalkOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Character::mindTalkOperation" << std::endl << std::flush
    ;);
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Look operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindLookOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got look up from mind from [" << op->getFrom() << "] to [" << op->getTo() << "]" << std::endl << std::flush
    ;);
    m_flags |= entity_perceptive;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        //If nothing is specified, send to parent, if available.
        if (m_location.m_loc) {
            op->setTo(m_location.m_loc->getId());
        } else {
            return;
        }
    } else {
        const Root & arg = args.front();
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            log(ERROR, describeEntity() + " mindLookOperation: Op has no ID");
            return;
        }
        op->setTo(arg->getId());
    }
    debug(std::cout << "  now to [" << op->getTo() << "]" << std::endl << std::flush
    ;);
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
        log(ERROR, describeEntity() + " mindEatOperation: Op has no ARGS");
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, describeEntity() + " mindEatOperation: Arg has no ID");
        return;
    }
    op->setTo(arg->getId());
    res.push_back(op);
}

/// \brief Filter a GoalInfo operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindGoalInfoOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
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
        log(ERROR, describeEntity() + " mindTouchOperation: Op has no ID");
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

/// \brief Filter any other operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindOtherOperation(const Operation & op, OpVector & res)
{
    log(WARNING, String::compose("Passing '%1' op from mind through to world. %2", op->getParents().front(), describeEntity()));
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

/// \brief Filter a Thought operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindThoughtOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

/// \brief Filter a Think operation coming from the mind
///
/// @param op The operation to be filtered.
/// @param res The filtered result is returned here.
void Character::mindThinkOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
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

/// \brief Filter a Thought operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mThoughtOperation(const Operation & op)
{
    //Only allow thoughts which are sent from the mind
    return op->getFrom() == getId();
}

bool Character::w2mThinkOperation(const Operation & op)
{
    return true;
}

bool Character::w2mCommuneOperation(const Operation & op)
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

/// \brief Filter a Relay operation coming from the world to the mind
///
/// @param op The operation to be filtered.
/// @return true if the operation should be passed.
bool Character::w2mRelayOperation(const Operation & op)
{
    //Relay is an internal op.
    return false;
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
    debug(std::cout << "Character::sendMind(" << op->getParents().front() << ") " << describeEntity() << std::endl << std::flush
    ;);

    if (m_externalMind != nullptr && m_externalMind->isLinked()) {
        OpVector mindRes;
        m_proxyMind->operation(op, mindRes);
        // Discard all the local results

        debug(std::cout << "Sending to external mind" << std::endl << std::flush
        ;);
        m_externalMind->operation(op, res);
    } else {
        debug(std::cout << "Using ops from local mind" << std::endl << std::flush
        ;);
        m_proxyMind->operation(op, res);
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
    debug(std::cout << "Character::mind2body(" << op->getParents().front() << ") " << describeEntity() << std::endl << std::flush
    ;);

    //Check if we have any relays registered for this op.
    if (!op->isDefaultRefno()) {
        auto I = m_relays.find(op->getRefno());
        if (I != m_relays.end()) {
            //The operation is a response to a relayed op, and we should send it on to the originating
            //entity. When doing this, we're basically wrapping an unsafe operation (i.e. the operation from
            //the mind could be anything), so it's important that the client or code which receives
            //the relayed op handles it with case.

            //Wrap the operation in a relay op
            Atlas::Objects::Operation::Generic relayOp;
            relayOp->setType("relay", Atlas::Objects::Operation::RELAY_NO);
            relayOp->setArgs1(op);
            relayOp->setTo(I->second.destination);
            relayOp->setRefno(I->second.serialno);
            res.push_back(relayOp);
            m_relays.erase(I);

            //The operation should not be processed anymore after it has been relayed.
            return;
        }
    }

    if (!op->isDefaultTo()) {

        log(ERROR, String::compose("Operation \"%1\" from mind with TO set. %2", op->getParents().front(), describeEntity()));
        return;
    }
    if (!op->isDefaultFutureSeconds() && op->getClassNo() != Atlas::Objects::Operation::TICK_NO) {
        log(ERROR, String::compose("Operation \"%1\" from mind with "
                "FUTURE_SECONDS set. %2", op->getParents().front(), describeEntity()));
    }
    auto op_no = op->getClassNo();
    switch (op_no) {
    case Atlas::Objects::Operation::COMBINE_NO:
        mindCombineOperation(op, res);
        break;
    case Atlas::Objects::Operation::CREATE_NO:
        mindCreateOperation(op, res);
        break;
    case Atlas::Objects::Operation::DELETE_NO:
        mindDeleteOperation(op, res);
        break;
    case Atlas::Objects::Operation::DIVIDE_NO:
        mindDivideOperation(op, res);
        break;
    case Atlas::Objects::Operation::IMAGINARY_NO:
        mindImaginaryOperation(op, res);
        break;
    case Atlas::Objects::Operation::LOOK_NO:
        mindLookOperation(op, res);
        break;
    case Atlas::Objects::Operation::MOVE_NO:
        mindMoveOperation(op, res);
        break;
    case Atlas::Objects::Operation::SET_NO:
        mindSetOperation(op, res);
        break;
    case Atlas::Objects::Operation::TALK_NO:
        mindTalkOperation(op, res);
        break;
    case Atlas::Objects::Operation::TOUCH_NO:
        mindTouchOperation(op, res);
        break;
    case Atlas::Objects::Operation::USE_NO:
        mindUseOperation(op, res);
        break;
    case Atlas::Objects::Operation::WIELD_NO:
        mindWieldOperation(op, res);
        break;
    default:
        if (op_no == Atlas::Objects::Operation::ACTUATE_NO) {
            mindActuateOperation(op, res);
        } else if (op_no == Atlas::Objects::Operation::ATTACK_NO) {
            mindAttackOperation(op, res);
        } else if (op_no == Atlas::Objects::Operation::EAT_NO) {
            mindEatOperation(op, res);
        } else if (op_no == Atlas::Objects::Operation::SETUP_NO) {
            mindSetupOperation(op, res);
        } else if (op_no == Atlas::Objects::Operation::TICK_NO) {
            mindTickOperation(op, res);
        } else if (op_no == Atlas::Objects::Operation::UPDATE_NO) {
            mindUpdateOperation(op, res);
        } else if (op_no == Atlas::Objects::Operation::THOUGHT_NO) {
            mindThoughtOperation(op, res);
        } else if (op_no == Atlas::Objects::Operation::GOAL_INFO_NO) {
            mindGoalInfoOperation(op, res);
        } else if (op_no == Atlas::Objects::Operation::THINK_NO) {
            mindThinkOperation(op, res);
        } else {
            mindOtherOperation(op, res);
        }
        break;
    }
}

/// \brief Filter operations from the world to the mind
///
/// Operations from the world are checked here to see if they are suitable
/// to send to the mind. Some operations should not go to the mind as they
/// leak information. Others are just not necessary as they provide no
/// useful information.
bool Character::world2mind(const Operation & op)
{
    debug(std::cout << "Character::world2mind(" << op->getParents().front() << ") " << describeEntity() << std::endl << std::flush
    ;);
    auto otype = op->getClassNo();
    POLL_OP_SWITCH(op, otype, w2m)
    return false;
}

void Character::filterExternalOperation(const Operation & op)
{
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

void Character::operation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Character::operation(" << op->getParents().front() << ") " << describeEntity() << std::endl << std::flush
    ;);
    Entity::operation(op, res);
    if (world2mind(op)) {
        debug(std::cout << "Character::operation(" << op->getParents().front() << ") passed to mind" << std::endl << std::flush
        ;);
        OpVector mres;
        sendMind(op, mres);
        OpVector::const_iterator Iend = mres.end();
        for (OpVector::const_iterator I = mres.begin(); I != Iend; ++I) {
            filterExternalOperation(*I);
        }
    }
}

void Character::externalOperation(const Operation & op, Link & link)
{
    debug(std::cout << "Character::externalOperation(" << op->getParents().front() << ") " << describeEntity() << std::endl << std::flush
    ;);
    if (linkExternal(&link) == 0) {
        debug(std::cout << "Subscribing existing character" << std::endl << std::flush
        ;);

        Info info;
        Anonymous info_arg;
        addToEntity(info_arg);
        info->setArgs1(info_arg);

        link.send(info);

        logEvent(TAKE_CHAR, String::compose("%1 - %2 Taken character (%3)", getId(), link.getId(), m_type ? m_type->name() : "none"));
    }
    filterExternalOperation(op);
}
