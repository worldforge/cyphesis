#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>


#include <common/log.h>

#include "Account.h"
#include "Connection.h"


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
    //type_class=get_thing_class(ent.type[0],"character");
    //obj=Account::add_character(type_class,ent);
    //log.inform("Player "+Account::id+" adds character "+`obj`,op);
    //return Operation("info", obj.as_entity());
    return(NULL);
}

Thing * Account::add_character(string character_class, const Message::Object & ent)
{
    //char=Account::world.add_object(character_class, ent);
    //if (not char.location) {
        //char.location=Location(parent=Account::world,;
                               //coordinates=\;
                               //Vector3D(randint(0,9),randint(0,4),0.0));
    //}
    //char.player=this;
    //char.external_mind=ExternalMind(id=char.id, body=char,;
                                    //connection=Account::connection);
    //Account::characters_dict[char.id]=char;
    //Account::characters.append(char.id);
    //Account::connection.add_object(char);
    //Account::world.message(Operation("sight",;
                                 //Operation("create",char.as_entity())),;
                       //char);
    //return char;
    return(NULL);
}
