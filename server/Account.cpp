// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
#include "common/BaseWorld.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Set.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Create;

static const bool debug_flag = false;

Account::Account(Connection * conn, const std::string & uname,
                 const std::string& passwd, const std::string & id)
                 : OOGThing(id), m_connection(conn),
                   m_username(uname), m_password(passwd)
{
    subscribe("logout", OP_LOGOUT);
    subscribe("create", OP_CREATE);
    subscribe("imaginary", OP_IMAGINARY);
    subscribe("talk", OP_TALK);
    subscribe("look", OP_LOOK);
    subscribe("set", OP_SET);
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
                                  const MapType & ent)
{
    BaseWorld & world = m_connection->m_server.m_world;
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    Entity * chr = world.addNewObject(typestr, ent);
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
    MapType entmap;
    entmap["parents"] = ListType(1,"coin");
    entmap["pos"] = Vector3D(0,0,0).toAtlas();
    entmap["loc"] = chr->getId();
    entmap["name"] = "coin";
    for(int i = 0; i < 10; i++) {
        Create * c = new Create;
        ListType & args = c->getArgs();
        args.push_back(entmap);
        c->setTo(chr->getId());
        c->setSerialno(newSerialNo());
        world.message(*c, chr);
    }

    return chr;
}

void Account::LogoutOperation(const Operation & op, OpVector &)
{
    debug(std::cout << "Account logout: " << getId() << std::endl;);
    Info info;
    ListType & args = info.getArgs();
    args.push_back(op.asObject());
    info.setRefno(op.getSerialno());
    info.setSerialno(newSerialNo());
    info.setFrom(getId());
    info.setTo(getId());
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

void Account::CreateOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Account::Operation(create)" << std::endl << std::flush;);
    const ListType & args = op.getArgs();
    if ((args.empty()) || (!args.front().isMap())) {
        return;
    }

    const MapType & entmap = args.front().asMap();

    if (characterError(op, entmap, res)) {
        return;
    }

    MapType::const_iterator I = entmap.find("parents");
    if ((I == entmap.end()) || !(I->second.isList()) ||
        (I->second.asList().empty()) ||
        !(I->second.asList().front().isString()) ) {
        error(op, "Character has no type", res, getId());
        return;
    }
    
    const std::string & typestr = I->second.asList().front().asString();
    debug( std::cout << "Account creating a " << typestr << " object"
                     << std::endl << std::flush; );

    BaseEntity * obj = addNewCharacter(typestr, entmap);

    if (obj == 0) {
        error(op, "Character creation failed", res, getId());
        return;
    }

    Info * info = new Info;
    ListType & info_args = info->getArgs();
    info_args.push_back(MapType());
    obj->addToMessage(info_args.front().asMap());
    info->setRefno(op.getSerialno());
    info->setSerialno(newSerialNo());

    res.push_back(info);
}

void Account::SetOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Account::Operation(set)" << std::endl << std::flush;);
    const ListType & args = op.getArgs();
    if ((args.empty()) || (!args.front().isMap())) {
        return;
    }

    const MapType & entmap = args.front().asMap();

    MapType::const_iterator I = entmap.find("id");
    if (I == entmap.end() || !(I->second.isString())) {
        error(op, "Set character has no ID", res, getId());
        return;
    }

    const std::string & id = I->second.asString();
    EntityDict::const_iterator J = m_charactersDict.find(id);
    if (J == m_charactersDict.end()) {
        return error(op, "Set character for unknown character", res, getId());
    }

    Entity * e = J->second;
    I = entmap.find("guise");
    MapType newArg;
    if (I != entmap.end()) {
        debug(std::cout << "Got attempt to change characters guise"
                        << std::endl << std::flush;);
        // Apply change to character in-game
        newArg["guise"] = I->second;
    }
    I = entmap.find("height");
    if (I != entmap.end() && (I->second.isNum())) {
        debug(std::cout << "Got attempt to change characters height"
                        << std::endl << std::flush;);
        BBox & bbox = e->m_location.m_bBox;
        if (bbox.isValid()) {
            float old_height = bbox.highCorner().z() - bbox.lowCorner().z();
            float scale = I->second.asNum() / old_height;
            BBox newBox(WFMath::Point<3>(bbox.lowCorner().x() * scale,
                                         bbox.lowCorner().y() * scale,
                                         bbox.lowCorner().z() * scale),
                        WFMath::Point<3>(bbox.highCorner().x() * scale,
                                         bbox.highCorner().y() * scale,
                                         bbox.highCorner().z() * scale));
            newArg["bbox"] = newBox.toAtlas();
        }
    }

    if (!newArg.empty()) {
        debug(std::cout << "Passing character mods in-game"
                        << std::endl << std::flush;);
        Set * s = new Set;
        s->setTo(id);
        newArg["id"] = id;
        ListType & sarg = s->getArgs();
        sarg.push_back(newArg);
        m_connection->m_server.m_world.message(*s, e);
    }
}

void Account::ImaginaryOperation(const Operation & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    if ((args.empty()) || (!args.front().isMap())) {
        return;
    }

    Sight s;
    ListType & sargs = s.getArgs();
    sargs.push_back(op.asObject());
    s.setFrom(getId());
    s.setSerialno(newSerialNo());
    s.setRefno(op.getSerialno());
    const MapType & arg = args.front().asMap();
    MapType::const_iterator I = arg.find("loc");
    if (I != arg.end()) {
        s.setTo(I->second.asString());
    } else {
        s.setTo(op.getTo());
    }
    m_connection->m_server.m_lobby.operation(s, res);
}

void Account::TalkOperation(const Operation & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    if ((args.empty()) || (!args.front().isMap())) {
        return;
    }

    Sound s;
    ListType & sargs = s.getArgs();
    sargs.push_back(op.asObject());
    s.setFrom(getId());
    s.setSerialno(newSerialNo());
    s.setRefno(op.getSerialno());
    const MapType & arg = args.front().asMap();
    MapType::const_iterator I = arg.find("loc");
    if (I != arg.end()) {
        s.setTo(I->second.asString());
    } else {
        s.setTo(op.getTo());
    }
    m_connection->m_server.m_lobby.operation(s, res);
}

void Account::LookOperation(const Operation & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    if (args.empty()) {
        Sight * s = new Sight;
        s->setTo(getId());
        ListType & s_args = s->getArgs();
        s_args.push_back(MapType());
        m_connection->m_server.m_lobby.addToMessage(s_args.front().asMap());
        s->setSerialno(newSerialNo());
        s->setRefno(op.getSerialno());
        res.push_back(s);
        return;
    }
    MapType::const_iterator I = args.front().asMap().find("id");
    if ((I == args.front().asMap().end()) || (!I->second.isString())) {
        error(op, "No target for look", res, getId());
        return;
    }
    const std::string & to = I->second.asString();
    EntityDict::const_iterator J = m_charactersDict.find(to);
    if (J != m_charactersDict.end()) {
        Sight * s = new Sight;
        s->setTo(getId());
        ListType & s_args = s->getArgs();
        s_args.push_back(MapType());
        J->second->addToMessage(s_args.front().asMap());
        s->setSerialno(newSerialNo());
        s->setRefno(op.getSerialno());
        res.push_back(s);
        return;
    }
    const AccountDict & accounts = m_connection->m_server.m_lobby.getAccounts();
    AccountDict::const_iterator K = accounts.find(to);
    if (K != accounts.end()) {
        Sight * s = new Sight;
        s->setTo(getId());
        ListType & s_args = s->getArgs();
        s_args.push_back(MapType());
        K->second->addToMessage(s_args.front().asMap());
        s->setSerialno(newSerialNo());
        s->setRefno(op.getSerialno());
        res.push_back(s);
        return;
    }
    error(op, "Unknown look target", res, getId());
}
