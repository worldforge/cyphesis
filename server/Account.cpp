// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#include "Account.h"

#include "Connection_methods.h"
#include "ServerRouting.h"
#include "Lobby.h"
#include "ExternalMind.h"
#include "Persistance.h"

#include "rulesets/Character.h"

#include "common/const.h"
#include "common/log.h"
#include "common/serialno.h"
#include "common/debug.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

Account::Account(Connection * conn, const std::string & uname,
                 const std::string& passwd, const std::string & id)
                 : OOGThing(id), m_connection(conn),
                   m_username(uname), m_password(passwd)
{
}

Account::~Account()
{
    ConMap::const_iterator J = m_destroyedConnections.begin();
    ConMap::const_iterator Jend = m_destroyedConnections.end();
    for (; J != Jend; ++J) {
        J->second->disconnect();
        delete J->second;
    }
}

void Account::characterDestroyed(std::string id)
{
    m_charactersDict.erase(id);
    ConMap::iterator I = m_destroyedConnections.find(id);
    if (I != m_destroyedConnections.end()) {
        delete I->second;
        m_destroyedConnections.erase(I);
    }
    if (consts::enable_persistence) {
        Persistance::instance()->delCharacter(id);
    }
}

void Account::addCharacter(Entity * chr)
{
    Character * pchar = dynamic_cast<Character *>(chr);
    if (pchar == 0) {
        return;
    }
    m_charactersDict[chr->getId()] = chr;
    SigC::Connection * con = new SigC::Connection(chr->destroyed.connect(SigC::bind<std::string>(SigC::slot(*this, &Account::characterDestroyed), chr->getId())));
    m_destroyedConnections[chr->getId()] = con;
}

Entity * Account::addNewCharacter(const std::string & typestr,
                                  const RootEntity & ent)
{
    assert(m_connection != 0);
    BaseWorld & world = m_connection->m_server.m_world;
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    Entity * chr = world.addNewEntity(typestr, ent);
    if (chr == 0) {
        return 0;
    }
    debug(std::cout << "Added" << std::endl << std::flush;);
    assert(chr->m_location.isValid());
    debug(std::cout << "Location set to: " << chr->m_location << std::endl << std::flush;);
    Character * pchar = dynamic_cast<Character *>(chr);
    if (pchar != 0) {
        pchar->m_externalMind = new ExternalMind(*m_connection, pchar->getId());
        // Only genuinely playable characters should go in here. Otherwise
        // if a normal entity gets into the account, and connection, it
        // starts getting hard to tell whether or not they exist.
        m_charactersDict[chr->getId()] = chr;
        SigC::Connection * con = new SigC::Connection(chr->destroyed.connect(SigC::bind<std::string>(SigC::slot(*this, &Account::characterDestroyed), chr->getId())));
        m_destroyedConnections[chr->getId()] = con;
        m_connection->addObject(chr);
        if (consts::enable_persistence) {
            Persistance::instance()->addCharacter(*this, *chr);
        }
    }

    // Hack in default objects
    // This needs to be done in a generic way
    Anonymous create_arg;
    create_arg->setParents(std::list<std::string>(1,"coin"));
    ::addToEntity(Point3D(0,0,0), create_arg->modifyPos());
    create_arg->setLoc(chr->getId());
    create_arg->setName("coin");
    // FIXME We can probably send the same op 10 times, rather than create 10
    // FIXME alternatively we can set 10 args on one op
    for(int i = 0; i < 10; i++) {
        Create c;
        c->setTo(chr->getId());
        c->setSerialno(newSerialNo());
        c->setArgs1(create_arg);
        world.message(c, *chr);
    }

    return chr;
}

void Account::LogoutOperation(const Operation & op, OpVector &)
{
    debug(std::cout << "Account logout: " << getId() << std::endl;);

    // It is possible this account is being forcibly logged out at a time
    // when it is not logged in.
    if (m_connection == 0) {
        return;
    }

    Info info;
    info->setArgs1(op);
    info->setRefno(op->getSerialno());
    info->setSerialno(newSerialNo());
    info->setFrom(getId());
    info->setTo(getId());
    // FIXME Direct send rather than reply - requires local refno handling
    m_connection->send(info);
    m_connection->disconnect();
}

const char * Account::getType() const
{
    return "account";
}

void Account::addToMessage(MapType & omap) const
{
    omap["username"] = m_username;
    omap["name"] = m_username;
    if (!m_password.empty()) {
        omap["password"] = m_password;
    }
    omap["parents"] = ListType(1,getType());
    ListType charlist;
    EntityDict::const_iterator I = m_charactersDict.begin();
    EntityDict::const_iterator Iend = m_charactersDict.end();
    for (; I != Iend; ++I) {
        charlist.push_back(I->first);
    }
    omap["characters"] = charlist;
    BaseEntity::addToMessage(omap);
}

void Account::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    ent->setAttr("username", m_username);
    ent->setName(m_username);
    if (!m_password.empty()) {
        ent->setAttr("password", m_password);
    }
    ent->setParents(std::list<std::string>(1,getType()));
    ListType charlist;
    EntityDict::const_iterator I = m_charactersDict.begin();
    EntityDict::const_iterator Iend = m_charactersDict.end();
    for (; I != Iend; ++I) {
        charlist.push_back(I->first);
    }
    ent->setAttr("characters", charlist);
    BaseEntity::addToEntity(ent);
}

void Account::CreateOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Account::Operation(create)" << std::endl << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());

    if (!arg.isValid()) {
        error(op, "Character creation arg is malformed", res, getId());
        return;
    }

    if (characterError(op, arg, res)) {
        return;
    }

    if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG) ||
        arg->getParents().empty()) {
        error(op, "Character has no type", res, getId());
        return;
    }
    
    const std::string & typestr = arg->getParents().front();
    debug( std::cout << "Account creating a " << typestr << " object"
                     << std::endl << std::flush; );

    BaseEntity * obj = addNewCharacter(typestr, arg);

    if (obj == 0) {
        error(op, "Character creation failed", res, getId());
        return;
    }

    Info info;
    Anonymous info_arg;
    obj->addToEntity(info_arg);
    info->setArgs1(info_arg);
    info->setSerialno(newSerialNo());

    res.push_back(info);
}

void Account::SetOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Account::Operation(set)" << std::endl << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    const Root & arg = args.front();

    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Set character has no ID", res, getId());
        return;
    }

    const std::string & id = arg->getId();
    EntityDict::const_iterator J = m_charactersDict.find(id);
    if (J == m_charactersDict.end()) {
        return error(op, "Set character for unknown character", res, getId());
    }

    Entity * e = J->second;
    Anonymous new_arg;
    bool argument_valid = false;
    Element guise;
    if (arg->copyAttr("guise", guise) == 0) {
        debug(std::cout << "Got attempt to change characters guise"
                        << std::endl << std::flush;);
        // Apply change to character in-game
        new_arg->setAttr("guise", guise);
        argument_valid = true;
    }
    Element height;
    if (arg->copyAttr("height", height) == 0 && (height.isNum())) {
        debug(std::cout << "Got attempt to change characters height"
                        << std::endl << std::flush;);
        const BBox & bbox = e->m_location.bBox();
        if (bbox.isValid()) {
            float old_height = bbox.highCorner().z() - bbox.lowCorner().z();
            float scale = height.asNum() / old_height;
            BBox newBox(WFMath::Point<3>(bbox.lowCorner().x() * scale,
                                         bbox.lowCorner().y() * scale,
                                         bbox.lowCorner().z() * scale),
                        WFMath::Point<3>(bbox.highCorner().x() * scale,
                                         bbox.highCorner().y() * scale,
                                         bbox.highCorner().z() * scale));
            new_arg->setAttr("bbox", newBox.toAtlas());
            argument_valid = true;
        }
    }

    if (argument_valid) {
        debug(std::cout << "Passing character mods in-game"
                        << std::endl << std::flush;);
        Set s;
        s->setTo(id);
        new_arg->setId(id);
        s->setArgs1(new_arg);
        assert(m_connection != 0);
        m_connection->m_server.m_world.message(s, *e);
    }
}

void Account::ImaginaryOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    Sight s;
    s->setArgs1(op);
    s->setFrom(getId());
    s->setSerialno(newSerialNo());
    // FIXME Remove this
    s->setRefno(op->getSerialno());
    RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());

    if (!arg.isValid()) {
        error(op, "Imaginary arg is malformed", res, getId());
        return;
    }

    if (arg->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        s->setTo(arg->getLoc());
    } else {
        s->setTo(op->getTo());
    }
    assert(m_connection != 0);
    m_connection->m_server.m_lobby.operation(s, res);
}

void Account::TalkOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    Sound s;
    s->setArgs1(op);
    s->setFrom(getId());
    s->setSerialno(newSerialNo());
    // FIXME Remove this - no really
    s->setRefno(op->getSerialno());
    RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());

    if (!arg.isValid()) {
        error(op, "Talk arg is malformed", res, getId());
        return;
    }

    if (arg->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        s->setTo(arg->getLoc());
    } else {
        s->setTo(op->getTo());
    }
    assert(m_connection != 0);
    m_connection->m_server.m_lobby.operation(s, res);
}

void Account::LookOperation(const Operation & op, OpVector & res)
{
    assert(m_connection != 0);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        m_connection->m_server.m_lobby.addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        s->setSerialno(newSerialNo());
        res.push_back(s);
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "No target for look", res, getId());
        return;
    }
    const std::string & to = arg->getId();
    EntityDict::const_iterator J = m_charactersDict.find(to);
    if (J != m_charactersDict.end()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        J->second->addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        s->setSerialno(newSerialNo());
        res.push_back(s);
        return;
    }
    const AccountDict & accounts = m_connection->m_server.m_lobby.getAccounts();
    AccountDict::const_iterator K = accounts.find(to);
    if (K != accounts.end()) {
        Sight s;
        s->setTo(getId());
        Anonymous sight_arg;
        K->second->addToEntity(sight_arg);
        s->setArgs1(sight_arg);
        s->setSerialno(newSerialNo());
        res.push_back(s);
        return;
    }
    error(op, "Unknown look target", res, getId());
}
