#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Info.h>


#include <common/log.h>

#include "Account.h"
#include "Connection.h"
#include "WorldRouter.h"


RootOperation * Account::Operation(const Logout & op)
{
    cout << "Account logout: " << name << endl;
    connection->disconnect();
    return(NULL);
}

void Account::addObject(Message::Object * obj)
{
    Message::Object::MapType & omap = obj->AsMap();
    if (password.size() != 0) {
        omap["password"] = Message::Object(password);
    }
    BaseEntity::addObject(obj);
}

RootOperation * Account::Operation(const Create & op)
{
    cout << "Account::Operation(create)" << endl << flush;
    const Message::Object & ent = op.GetArgs().front();
    if (!ent.IsMap()) {
        return error(op, "Invalid character");
    }
    Message::Object::MapType entmap = ent.AsMap();
    if ((entmap.find("parents")==entmap.end()) ||
        !entmap["parents"].IsList() ||
        (entmap["parents"].AsList().size()==0) ||
        !entmap["parents"].AsList().front().IsString() ) {
        return error(op, "Character has no type");
    }
    
    RootOperation * error = character_error(op, ent);
    if (error) {
        return error;
    }
    const string & type = entmap["parents"].AsList().front().AsString();

    BaseEntity * obj = add_character(type, ent);
    //log.inform("Player "+Account::id+" adds character "+`obj`,op);
    //return Operation("info", obj.as_entity());
    Info * info = new Info();
    *info = Info::Instantiate();
    Message::Object::ListType args(1,obj->asObject());
    info->SetArgs(args);

    return(info);
}

BaseEntity * Account::add_character(const string & type, const Message::Object & ent)
{
    BaseEntity * chr = world->add_object(type, ent);
    if (!chr->location) {
        chr->location = Location(world, Vector3D(0,0,0));
    }
    //chr->player=this;
    //char.external_mind=ExternalMind(id=char.id, body=char,;
                                    //connection=Account::connection);
    //Account::characters_dict[char.id]=char;
    //Account::characters.append(char.id);
    connection->add_object(chr);
    //Account::world.message(Operation("sight",;
                                 //Operation("create",char.as_entity())),;
                       //char);
    //return char;
    return(chr);
}
