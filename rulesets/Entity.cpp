// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "Entity.h"

#include "Script.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/inheritance.h"
#include "common/Property.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

std::set<std::string> Entity::m_immutable;

const std::set<std::string> & Entity::immutables()
{
    if (m_immutable.empty()) {
        m_immutable.insert("parents");
        m_immutable.insert("pos");
        m_immutable.insert("loc");
        m_immutable.insert("velocity");
        m_immutable.insert("orientation");
        m_immutable.insert("contains");
    }
    return m_immutable;
}

Entity::Entity(const std::string & id) : BaseEntity(id),
                                         m_refCount(0), m_destroyed(false),
                                         m_script(new Script), m_seq(0),
                                         m_status(1), m_type("entity"),
                                         m_mass(-1), m_perceptive(false),
                                         m_world(NULL), m_update_flags(0)
{
    subscribe("create", OP_CREATE);

    m_properties["status"] = new Property<double>(m_status, a_status);
    m_properties["id"] = new ImmutableProperty<std::string>(getId());
    m_properties["name"] = new Property<std::string>(m_name, a_name);
    m_properties["mass"] = new Property<double>(m_mass, a_mass);
    m_properties["bbox"] = new Property<BBox>(m_location.m_bBox, a_bbox);
    m_properties["contains"] = new ImmutableProperty<EntitySet>(m_contains);
}

Entity::~Entity()
{
    if (m_script != NULL) {
        delete m_script;
    }
    PropertyDict::const_iterator I = m_properties.begin();
    PropertyDict::const_iterator Iend = m_properties.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }
    if (m_location.m_loc != 0) {
        m_location.m_loc->decRef();
    }
}

/// \brief Check if this entity has a property with the given name
///
/// @param aname Name of attribute to be checked
/// @return trye if this entity has an attribute with the name given
/// false otherwise
bool Entity::has(const std::string & aname) const
{
    PropertyDict::const_iterator I = m_properties.find(aname);
    if (I != m_properties.end()) {
        return true;
    }
    MapType::const_iterator J = m_attributes.find(aname);
    if (J != m_attributes.end()) {
        return true;
    }
    return false;
    
}

/// \brief Get the value of an attribute
///
/// @param aname Name of attribute to be retrieved
/// @param attr Reference used to store value
/// @return trye if this entity has an attribute with the name given
/// false otherwise
bool Entity::get(const std::string & aname, Element & attr) const
{
    PropertyDict::const_iterator I = m_properties.find(aname);
    if (I != m_properties.end()) {
        I->second->get(attr);
        return true;
    }
    MapType::const_iterator J = m_attributes.find(aname);
    if (J != m_attributes.end()) {
        attr = J->second;
        return true;
    }
    return false;
}

/// \brief Set the value of an attribute
///
/// @param aname Name of attribute to be changed
/// @param attr Value to be stored
void Entity::set(const std::string & aname, const Element & attr)
{
    PropertyDict::const_iterator I = m_properties.find(aname);
    if (I != m_properties.end()) {
        I->second->set(attr);
        m_update_flags |= I->second->flags();
        return;
    }
    m_attributes[aname] = attr;
    m_update_flags |= a_attr;
}

/// \brief Copy attributes into an Atlas element
///
/// @param omap Atlas map element this entity should be copied into
void Entity::addToMessage(MapType & omap) const
{
    // We need to have a list of keys to pull from attributes.
    MapType::const_iterator Iend = m_attributes.end();
    for (MapType::const_iterator I = m_attributes.begin(); I != Iend; ++I) {
        omap[I->first] = I->second;
    }
    PropertyDict::const_iterator J = m_properties.begin();
    PropertyDict::const_iterator Jend = m_properties.end();
    for (; J != Jend; ++J) {
        J->second->add(J->first, omap);
    }
    omap["stamp"] = (double)m_seq;
    omap["parents"] = ListType(1, m_type);
    m_location.addToMessage(omap);
    omap["objtype"] = "obj";
}

/// \brief Copy attributes into an Atlas entity
///
/// @param ent Atlas entity this entity should be copied into
void Entity::addToEntity(const RootEntity & ent) const
{
    // We need to have a list of keys to pull from attributes.
    MapType::const_iterator Iend = m_attributes.end();
    for (MapType::const_iterator I = m_attributes.begin(); I != Iend; ++I) {
        ent->setAttr(I->first, I->second);
    }
    PropertyDict::const_iterator J = m_properties.begin();
    PropertyDict::const_iterator Jend = m_properties.end();
    for (; J != Jend; ++J) {
        Element val;
        J->second->get(val);
        ent->setAttr(J->first, val);
    }
    ent->setAttr("stamp", (double)m_seq);
    ent->setParents(std::list<std::string>(1, m_type));
    m_location.addToEntity(ent);
    ent->setObjtype("obj");
}

/// \brief Associate a script with this entity
///
/// The previously associated script is deleted.
/// @param scrpt Pointer to the script to be associated with this entity
void Entity::setScript(Script * scrpt)
{
    if (m_script != NULL) {
        delete m_script;
    }
    m_script = scrpt;
}

/// \brief Destroy this entity
///
/// Do the jobs required to remove this entity from the world. Handles
/// removing from the containership tree.
void Entity::destroy()
{
    assert(m_location.m_loc != NULL);
    EntitySet & refContains = m_location.m_loc->m_contains;
    EntitySet::const_iterator Iend = m_contains.end();
    for (EntitySet::const_iterator I = m_contains.begin(); I != Iend; ++I) {
        Entity * obj = *I;
        // FIXME take account of orientation
        // FIXME velocity and orientation  need to be adjusted
        obj->m_location.m_loc = m_location.m_loc;
        m_location.m_loc->incRef();
        if (m_location.m_orientation.isValid()) {
            obj->m_location.m_pos = obj->m_location.m_pos.toParentCoords(m_location.m_pos, m_location.m_orientation);
            obj->m_location.m_velocity.rotate(m_location.m_orientation);
            obj->m_location.m_orientation *= m_location.m_orientation;
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            obj->m_location.m_pos = obj->m_location.m_pos.toParentCoords(m_location.m_pos, identity);
        }
        refContains.insert(obj);
    }
    refContains.erase(this);

    // We don't call decRef() on our parent, because we may not get deleted
    // yet, and we need to keep a reference to our parent in case there
    // are broadcast ops left that we have not yet sent.

    if (m_location.m_loc->m_contains.empty()) {
        m_location.m_loc->m_update_flags |= a_cont;
        m_location.m_loc->updated.emit();
    }
    m_destroyed = true;
    destroyed.emit();
}

/// \brief Read attributes from an Atlas element
///
/// @param ent The Atlas map element containing the attribute values
void Entity::merge(const MapType & ent)
{
    const std::set<std::string> & imm = immutables();
    MapType::const_iterator Iend = ent.end();
    for (MapType::const_iterator I = ent.begin(); I != Iend; ++I) {
        const std::string & key = I->first;
        if (imm.find(key) != imm.end()) continue;
        set(key, I->second);
    }
}

/// \brief Subscribe this entity to operations of the type given
///
/// @param op Type of operation this entity should be subscribed to
void Entity::scriptSubscribe(const std::string & op)
{
    OpNo n = Inheritance::instance().opEnumerate(op);
    if (n != OP_INVALID) {
        debug(std::cout << "SCRIPT requesting subscription to " << op
                        << std::endl << std::flush;);
        subscribe(op, n);
    } else {
        std::string msg = std::string("SCRIPT requesting subscription to ")
                        + op + " but inheritance could not give me a reference";
        log(ERROR, msg.c_str());
    }
}

/// \brief Process an operation from an external source.
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Entity::externalOperation(const Operation & op)
{
    OpVector res;
    operation(op, res);
    OpVector::const_iterator Iend = res.end();
    for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
        (*I)->setRefno(op->getSerialno());
        sendWorld(*I);
    }
}

void Entity::SetupOperation(const Operation & op, OpVector & res)
{
    m_script->operation("setup", op, res);
}

void Entity::TickOperation(const Operation & op, OpVector & res)
{
    m_script->operation("tick", op, res);
}

void Entity::ActionOperation(const Operation & op, OpVector & res)
{
    m_script->operation("action", op, res);
}

void Entity::ChopOperation(const Operation & op, OpVector & res)
{
    m_script->operation("chop", op, res);
}

void Entity::CreateOperation(const Operation & op, OpVector & res)
{
    if (m_script->operation("create", op, res) != 0) {
        return;
    }
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
       return;
    }
    try {
        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
        if (!ent.isValid()) {
            error(op, "Entity to be created is malformed", res, getId());
            return;
        }
        const std::list<std::string> & parents = ent->getParents();
        if (parents.empty()) {
            error(op, "Entity to be created has empty parents", res, getId());
            return;
        }
        if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG) &&
            (m_location.m_loc != 0)) {
            ent->setLoc(m_location.m_loc->getId());
            if (!ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
                ::addToEntity(m_location.m_pos, ent->modifyPos());
            }
        }
        const std::string & type = parents.front();
        debug( std::cout << getId() << " creating " << type;);

        Entity * obj = m_world->addNewEntity(type,ent);

        if (obj == 0) {
            error(op, "Create op failed.", res, op->getFrom());
            return;
        }

        Operation c(op.copy());

        Anonymous new_ent;
        obj->addToEntity(new_ent);
        c->setArgs1(new_ent);

        Sight s;
        s->setArgs1(c);
        res.push_back(s);
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: Malformed object to be created");
        error(op, "Malformed object to be created", res, getId());
        return;
    }
}

void Entity::CutOperation(const Operation & op, OpVector & res)
{
    m_script->operation("cut", op, res);
}

void Entity::DeleteOperation(const Operation & op, OpVector & res)
{
    m_script->operation("delete", op, res);
}

void Entity::EatOperation(const Operation & op, OpVector & res)
{
    m_script->operation("eat", op, res);
}

void Entity::BurnOperation(const Operation & op, OpVector & res)
{
    m_script->operation("burn", op, res);
}

void Entity::ImaginaryOperation(const Operation & op, OpVector & res)
{
    m_script->operation("imaginary", op, res);
}

void Entity::MoveOperation(const Operation & op, OpVector & res)
{
    m_script->operation("move", op, res);
}

void Entity::NourishOperation(const Operation & op, OpVector & res)
{
    m_script->operation("nourish", op, res);
}

void Entity::SetOperation(const Operation & op, OpVector & res)
{
    m_script->operation("set", op, res);
}

void Entity::SightOperation(const Operation & op, OpVector & res)
{
    m_script->operation("sight", op, res);
}

void Entity::SoundOperation(const Operation & op, OpVector & res)
{
    m_script->operation("sound", op, res);
}

void Entity::TouchOperation(const Operation & op, OpVector & res)
{
    m_script->operation("touch", op, res);
}

void Entity::LookOperation(const Operation & op, OpVector & res)
{
    if (m_script->operation("look", op, res) != 0) {
        return;
    }

    Sight s;

    Anonymous new_ent;
    addToEntity(new_ent);
    s->setArgs1(new_ent);

    s->setTo(op->getFrom());

    res.push_back(s);
}

void Entity::AppearanceOperation(const Operation & op, OpVector & res)
{
    m_script->operation("appearance", op, res);
}

void Entity::DisappearanceOperation(const Operation & op, OpVector & res)
{
    m_script->operation("disappearance", op, res);
}

void Entity::OtherOperation(const Operation & op, OpVector & res)
{
    const std::string & op_type = op->getParents().front();
    debug(std::cout << "Entity " << getId() << " got custom " << op_type
                    << " op" << std::endl << std::flush;);
    m_script->operation(op_type, op, res);
}
