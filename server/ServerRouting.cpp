#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "ServerRouting.h"

#include <common/persistance.h>


ServerRouting::ServerRouting(CommServer * server, char * name) : svr_name(name)
{
    fullid = name;
    //ServerRouting::base_init(kw);
    comm_server=server ; //communication server;
    id_dict[fullid] = this;
    world=new WorldRouter(this); //game world;
    BaseEntity * obj=add_object((BaseEntity*)Persistance::load_admin_account());
    //obj->server=this;
    obj->world=world;
}

BaseEntity * ServerRouting::add_object(BaseEntity * obj) {
    obj=Routing::add_object(obj);
    id_dict[obj->fullid] = obj;
    return obj;
}

void save(char * filename)
{
    //persistence.save_meta(self, filename);
}

void load(char * filename)
{
    //persistence.load_meta(self, filename);
}

int ServerRouting::idle() {
    return world->idle();
}
