// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "inheritance_impl.h"

#include "log.h"

#include <Atlas/Objects/Operation.h>

#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;

using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Combine;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Divide;
using Atlas::Objects::Operation::Imaginary;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Touch;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Operation::Wield;

Inheritance * Inheritance::m_instance = NULL;

Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParents(std::list<std::string>(1, parent));
    r->setObjtype("op_definition");
    r->setId(name);

    return r;
}

Root atlasClass(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParents(std::list<std::string>(1, parent));
    r->setObjtype("class");
    r->setId(name);

    return r;
}

Inheritance::Inheritance() : noClass(0)
{
    Atlas::Objects::Entity::Anonymous root;

    root->setParents(std::list<std::string>(0));
    root->setId("root");

    atlasObjects["root"] = root;
}

void Inheritance::flush()
{
    atlasObjects.clear();

    OpFactoryDict::const_iterator J = opFactories.begin();
    OpFactoryDict::const_iterator Jend = opFactories.end();
    for (; J != Jend; ++J) {
        delete J->second;
    }
    opFactories.clear();
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
        installStandardObjects();
        installCustomOperations();
        installCustomEntities();
    }
    return *m_instance;
}

void Inheritance::clear()
{
    if (m_instance != NULL) {
        m_instance->flush();
        delete m_instance;
    }
}

OpNo Inheritance::opEnumerate(const std::string & parent) const
{
    OpNoDict::const_iterator I = opLookup.find(parent);
    if (I != opLookup.end()) {
        return I->second;
    } else {
        return OP_INVALID;
    }
}

OpNo Inheritance::opEnumerate(const Operation & op) const
{
#warning This should no longer be necessary - just return the Atlas number for the class
    const std::list<std::string> & parents = op->getParents();
    if (parents.size() != 1) {
        log(ERROR, "op with no parents");
    }
    const std::string & parent = parents.front();
    return opEnumerate(parent);
}

const Atlas::Objects::Root & Inheritance::getClass(const std::string & parent)
{
    RootDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return noClass;
    }
    return I->second;
}

bool Inheritance::hasClass(const std::string & parent)
{
    RootDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return false;
    }
    return true;
}

int Inheritance::addChild(const Atlas::Objects::Root & obj)
{
    const std::string & child = obj->getId();
    const std::string & parent = obj->getParents().front();
    if (atlasObjects.find(child) != atlasObjects.end()) {
        std::string msg = std::string("Installing type ") + child 
                        + "(" + parent + ") which was already installed";
        log(ERROR, msg.c_str());
        return -1;
    }
    RootDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        std::string msg = std::string("Installing type ") + child 
                        + " which has unknown parent \"" + parent + "\"";
        log(ERROR, msg.c_str());
        return -1;
    }
    ListType children(1, child);
    if (I->second->hasAttr("children")) {
        children = I->second->getAttr("children").asList();
        children.push_back(child);
    }
    I->second->setAttr("children", Element(children));
    atlasObjects[child] = obj;
    return 0;
}

Operation Inheritance::newOperation(const std::string & op_type)
{
    OpFactoryDict::const_iterator I = opFactories.find(op_type);
    if (I == opFactories.end()) {
        return NULL;
    }
    return I->second->newOperation();
}

int Inheritance::newOperation(const std::string & op_type, Operation & ret) const
{
    OpFactoryDict::const_iterator I = opFactories.find(op_type);
    if (I == opFactories.end()) {
        return -1;
    }
    I->second->newOperation(ret);
    return 0;
}

bool Inheritance::isTypeOf(const std::string & instance,
                           const std::string & type) const
{
    if (instance == type) {
        return true;
    }
    RootDict::const_iterator I = atlasObjects.find(instance);
    RootDict::const_iterator Iend = atlasObjects.end();
    for (; I != Iend;) {
        const std::list<std::string> & parents = I->second->getParents();
        if (parents.empty()) {
            break;
        }
        const std::string & parent = I->second->getParents().front();
        if (parent == type) {
            return true;
        }
        I = atlasObjects.find(parent);
    }
    return false;
    // Walk up the tree.
}

using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Perception;
using Atlas::Objects::Operation::Communicate;
using Atlas::Objects::Operation::Perceive;
using Atlas::Objects::Operation::Smell;
using Atlas::Objects::Operation::Feel;
using Atlas::Objects::Operation::Listen;
using Atlas::Objects::Operation::Sniff;

using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::AdminEntity;
using Atlas::Objects::Entity::Game;
using Atlas::Objects::Entity::GameEntity;

OpFactoryBase::~OpFactoryBase()
{
}

GenericOpFactory::GenericOpFactory(const std::string &opType) : m_opType(opType)
{
}

Operation GenericOpFactory::newOperation()
{
    Root obj = Atlas::Objects::Factories::instance()->createObject(m_opType);
    return Atlas::Objects::smart_dynamic_cast<RootOperation>(obj);
}

void GenericOpFactory::newOperation(Operation & ret)
{
    Root obj = Atlas::Objects::Factories::instance()->createObject(m_opType);
    ret = Atlas::Objects::smart_dynamic_cast<RootOperation>(obj);
}

void installStandardObjects()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(atlasOpDefinition("root_operation", "root"));
    i.addChild(atlasOpDefinition("action", "root_operation"));
    i.opInstall("action", OP_ACTION, new OpFactory<Action>);
    i.addChild(atlasOpDefinition("create", "action"));
    i.opInstall("create", OP_CREATE, new OpFactory<Create>);
    i.addChild(atlasOpDefinition("delete", "action"));
    i.opInstall("delete", OP_DELETE, new OpFactory<Delete>);
    i.addChild(atlasOpDefinition("info", "root_operation"));
    i.opInstall("info", OP_INFO, new OpFactory<Info>);
    i.addChild(atlasOpDefinition("set", "action"));
    i.opInstall("set", OP_SET, new OpFactory<Set>);
    i.addChild(atlasOpDefinition("get", "action"));
    i.opInstall("get", OP_GET, new OpFactory<Get>);
    i.addChild(atlasOpDefinition("perception", "info"));
    i.addChild(atlasOpDefinition("error", "info"));
    i.opInstall("error", OP_ERROR, new OpFactory<Error>);
    i.addChild(atlasOpDefinition("combine", "create"));
    i.opInstall("combine", OP_COMBINE, new OpFactory<Combine>);
    i.addChild(atlasOpDefinition("divide", "create"));
    i.opInstall("divide", OP_DIVIDE, new OpFactory<Divide>);
    i.addChild(atlasOpDefinition("communicate", "create"));
    i.addChild(atlasOpDefinition("move", "set"));
    i.opInstall("move", OP_MOVE, new OpFactory<Move>);
    i.addChild(atlasOpDefinition("perceive", "get"));
    i.addChild(atlasOpDefinition("login", "get"));
    i.opInstall("login", OP_LOGIN, new OpFactory<Login>);
    i.addChild(atlasOpDefinition("logout", "login"));
    i.opInstall("logout", OP_LOGOUT, new OpFactory<Logout>);
    i.addChild(atlasOpDefinition("sight", "perception"));
    i.opInstall("sight", OP_SIGHT, new OpFactory<Sight>);
    i.addChild(atlasOpDefinition("sound", "perception"));
    i.opInstall("sound", OP_SOUND, new OpFactory<Sound>);
    i.addChild(atlasOpDefinition("smell", "perception"));
    i.addChild(atlasOpDefinition("feel", "perception"));
    i.addChild(atlasOpDefinition("imaginary", "action"));
    i.opInstall("imaginary", OP_IMAGINARY, new OpFactory<Imaginary>);
    i.addChild(atlasOpDefinition("talk", "communicate"));
    i.opInstall("talk", OP_TALK, new OpFactory<Talk>);
    i.addChild(atlasOpDefinition("look", "perceive"));
    i.opInstall("look", OP_LOOK, new OpFactory<Look>);
    i.addChild(atlasOpDefinition("listen", "perceive"));
    i.addChild(atlasOpDefinition("sniff", "perceive"));
    i.addChild(atlasOpDefinition("touch", "perceive"));
    i.opInstall("touch", OP_TOUCH, new OpFactory<Touch>);
    i.addChild(atlasOpDefinition("appearance", "sight"));
    i.opInstall("appearance", OP_APPEARANCE, new OpFactory<Appearance>);
    i.addChild(atlasOpDefinition("disappearance", "sight"));
    i.opInstall("disappearance", OP_DISAPPEARANCE, new OpFactory<Disappearance>);
    i.addChild(atlasOpDefinition("use", "foo"));
    i.opInstall("use", OP_USE, new OpFactory<Use>);
    i.addChild(atlasOpDefinition("wield", "foo"));
    i.opInstall("wield", OP_WIELD, new OpFactory<Wield>);



    i.addChild(atlasClass("root_entity", "root"));
    i.addChild(atlasClass("admin_entity", "root_entity"));
    i.addChild(atlasClass("account", "admin_entity"));
    i.addChild(atlasClass("player", "account"));
    i.addChild(atlasClass("admin", "account"));
    i.addChild(atlasClass("game", "admin_entity"));
    i.addChild(atlasClass("game_entity", "root_entity"));

    // And from here on we need to define the hierarchy as found in the C++
    // base classes. Script classes defined in rulsets need to be added
    // at runtime.
}
