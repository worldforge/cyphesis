#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "ServerRouting.h"


ServerRouting::ServerRouting(CommServer * server, char * name) : svr_name(name)
{
    //ServerRouting::base_init(kw);
    comm_server=server ; //communication server;
    id_dict[id] = this;
    world=new WorldRouter(this); //game world;
    //obj=add_object(persistence.load_admin_account());
    //obj.server=self;
    //obj.world=ServerRouting::world;
}

BaseEntity * ServerRouting::add_object(BaseEntity * obj) {
    obj=Routing::add_object(obj);
    id_dict[obj->id] = obj;
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
