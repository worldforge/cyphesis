#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Info.h>


#include <common/log.h>

#include <rulesets/Character.h>
#include <rulesets/ExternalMind.h>

#include "Account.h"
#include "Connection.h"
#include "WorldRouter.h"

static int debug_server = 0;

inline BaseEntity * Account::add_character(const string & typestr, const Message::Object & ent)
{
    debug_server && cout << "Account::Add_character" << endl << flush;
    Thing * chr = world->add_object(typestr, ent);
    debug_server && cout << "Added" << endl << flush;
    if (!chr->location) {
        debug_server && cout << "Setting location" << endl << flush;
        chr->location = Location(world, Vector3D(0,0,0));
    }
    debug_server && cout << "Location set to: " << chr->location << endl << flush;
    if (chr->is_character != 0) {
        Character * pchar = (Character *)chr;
        pchar->player = this;
        pchar->external_mind = new ExternalMind(connection, pchar->fullid, pchar->name);
    }
    characters_dict[chr->fullid]=chr;
    characters.push_back(chr->fullid);
    connection->add_object(chr);

    Create c = Create::Instantiate();

    list<Message::Object> cargs(1,chr->asObject());
    c.SetArgs(cargs);

    Sight * s = new Sight;
    *s = Sight::Instantiate();
    
    list<Message::Object> sargs(1,c.AsObject());
    s->SetArgs(sargs);

    world->message(*s, chr);

    return(chr);
}

oplist Account::Operation(const Logout & op)
{
    debug_server && cout << "Account logout: " << name << endl;
    connection->disconnect();
    oplist res;
    return(res);
}

void Account::addObject(Message::Object * obj)
{
    Message::Object::MapType & omap = obj->AsMap();
    if (password.size() != 0) {
        omap["password"] = Message::Object(password);
    }
    omap["parents"] = Message::Object(Message::Object::ListType(1,Message::Object(type)));
    Object::ListType charlist;
    list<string>::const_iterator I;
    for(I = characters.begin(); I != characters.end(); I++) {
        charlist.push_back(Object(*I));
    }
    omap["characters"] = Object(charlist);
    BaseEntity::addObject(obj);
}

oplist Account::Operation(const Create & op)
{
    debug_server && cout << "Account::Operation(create)" << endl << flush;
    const Message::Object & ent = op.GetArgs().front();
    if (!ent.IsMap()) {
        return(error(op, "Invalid character"));
    }
    Message::Object::MapType entmap = ent.AsMap();
    if ((entmap.find("parents")==entmap.end()) ||
        !(entmap["parents"].IsList()) ||
        (entmap["parents"].AsList().size()==0) ||
        !(entmap["parents"].AsList().front().IsString()) ) {
        return(error(op, "Character has no type"));
    }
    
    oplist error = character_error(op, ent);
    if (error.size() != 0) {
        return(error);
    }
    const string & typestr = entmap["parents"].AsList().front().AsString();
    debug_server && cout << "Account creating a " << typestr << " object" << endl << flush;

    BaseEntity * obj = add_character(typestr, ent);
    //log.inform("Player "+Account::id+" adds character "+`obj`,op);
    Info * info = new Info();
    *info = Info::Instantiate();
    Message::Object::ListType args(1,obj->asObject());
    info->SetArgs(args);
    info->SetRefno(op.GetSerialno());

    oplist res;
    res.push_back(info);
    return(res);
}
