// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommClient.h"

#include "CreatorClient.h"

#include "common/debug.h"
#include "common/BaseEntity.h"

static const bool debug_flag = false;

CommClient::CommClient()
{
}

Element::MapType CommClient::createPlayer(const std::string & name,
                                           const std::string & password)
{
    playerName = name;
    Element::MapType player_ent;
    player_ent["username"] = name;
    player_ent["password"] = password;
    player_ent["parents"] = Element::ListType(1, "player");
    
    debug(std::cout << "Loggin " << name << " in with " << password << " as password"
               << std::endl << std::flush;);
    
    Login loginAccountOp(Login::Instantiate());
    loginAccountOp.setArgs(Element::ListType(1,player_ent));
    send(loginAccountOp);

    if (connection.wait()) {
        Create createAccountOp(Create::Instantiate());
        createAccountOp.setArgs(Element::ListType(1,player_ent));
        send(createAccountOp);
        if (connection.wait()) {
            std::cerr << "ERROR: Failed to log into server" << std::endl
                      << std::flush;
            return Element::MapType();
        }
    }

    const Element::MapType & ent = connection.getReply();

    Element::MapType::const_iterator I = ent.find("id");
    if (I == ent.end() || !I->second.isString()) {
        std::cerr << "ERROR: Logged in, but account has no id" << std::endl
                  << std::flush;
    } else {
        playerId = I->second.asString();
    }
    //if (ent.find("characters") != ent.end()) {
    //}

    return ent;
}

CreatorClient * CommClient::createCharacter(const std::string & type)
{
    Element::MapType character;
    character["name"] = playerName;
    character["parents"] = Element::ListType(1,type);

    Create createOp=Create::Instantiate();
    createOp.setFrom(playerId);
    createOp.setArgs(Element::ListType(1,character));
    send(createOp);

    if (connection.wait()) {
        std::cerr << "ERROR: Failed to create character type: "
                  << type << std::endl << std::flush;
        return NULL;
    }
    const Element::MapType & body = connection.getReply();

    const std::string & id = body.find("id")->second.asString();

    EntityDict tmp;

    CreatorClient * obj = new CreatorClient(id, type, connection);
    obj->merge(body);
    obj->getLocation(body, tmp);
    // obj = EntityFactory::instance()->newThing(type, body, tmp);
    // FIXME Do we need to create a local entity for this as is done in
    // the python version? If so, do we need to keep track of a full world
    // model here, or just in the minds (when we become an AI client
    return obj;
}

// I'm making this pure virtual, to see if that is desired.
//void CommClient::idle() {
//    time.sleep(0.1);
//}

void CommClient::handleNet()
{
    RootOperation * input;
    while ((input = connection.pop()) != NULL) {
        OpVector result = character->message(*input);
        for (OpVector::const_iterator I=result.begin(); I != result.end(); I++) {
            send(*(*I));
        }
        delete input;
    }
}
