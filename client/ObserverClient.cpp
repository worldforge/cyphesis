// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ObserverClient.h"

#include "define_world.h"

#include <common/accountbase.h>
#include <common/const.h>

#include <iostream>

#include <unistd.h>

using Atlas::Message::Object;

ObserverClient::ObserverClient()
{
}

bool ObserverClient::setup()
{
    if (!connect()) {
        return false;
    }

    Object::MapType adminAccount;
    std::string password = consts::defaultAdminPassword;
    if (!AccountBase::instance()->getAccount("admin", adminAccount)) {
        std::cerr << "WARNING: Unable to read admin account from database"
                  << std::endl << "Using default"
                  << std::endl << std::flush;
    } else {
        Object::MapType::const_iterator I = adminAccount.find("password");
        if (I == adminAccount.end()) {
            std::cerr << "WARNING: Admin account from database has no password"
                      << std::endl << std::flush;
        } else {
            password = I->second.AsString();
        }
    }
    AccountBase::del();
    player = createPlayer("admin",password);
    if (player.empty()) {
        return false;
    }
    character = createCharacter("creator");
    if (character == NULL) {
        return false;
    }
    return true;
}

void ObserverClient::loadDefault()
{
    DefineWorld::define(character);
}

void ObserverClient::idle()
{
    usleep(100);
    // This is where we will put diagnostics, and maybe in future the AI code.
#if 0
    // time.sleep(0.1);
    if (not ObserverClient::display) {
        return;
    }
    ObserverClient::map=ObserverClient::character.map;
    wait=0.0;
    xmin=-300.0;
    xmax=-xmin;
    ymin=-150.0;
    ymax=-ymin;
    sizex=49;
    sizey=20;
    goal_width=30;
    screen = [None]*sizex;
    for (/*i in range(sizex)*/) {
        screen[i] = ['.'] * sizey;
    }
    house_list=[];
    mind_list=[];
    fire_list=[];
    for (/*t in ObserverClient::map.things.values()*/) {
        if (len(t.type)>=1) {
            typething = string.split(t.type[0], ".");
            if (string.find(typething[-1], "house")>=0) {
                house_list.append(t);
            }
            else if (string.find(typething[-1], "farmer")>=0 or string.find(typething[-1], "smith")>=0 or string.find(typething[-1], "character")>=0 or string.find(typething[-1], "creator")>=0) {
                mind_list.append(t);
            }
            else if (string.find(typething[-1], "fire")) {
                fire_list.append(t);
            }
        }
        else {
            print "CHEAT!: somewhere is entity without type!:",t;
        }
    }
    ObserverClient::time=str(ObserverClient::character.time);
    status_str=ObserverClient::time+" Count of minds: "+`len(mind_list)`;
    print chr(27)+"[H",status_str+" "*(sizex-len(status_str)-1);
    for (/*t in house_list*/) {
        (x,y,z)=t.get_xyz();
        screen[scx(x)][scy(y)] = t.name[0];
    }
    yind=1;
    goal_txt={};
    for (/*m in mind_list*/) {
        (x,y,z)=m.get_xyz();
        x,y=scx(x),scy(y);
        screen[x][y]=m.name[0];
        screen[x+1][y]=m.name[-1];
        if (hasattr(m,"goal")) {
            goal_txt[yind]=(`m`+":"+m.goal+" "*goal_width)[:goal_width];
            yind=yind+1;
        }
    }
    for (/*t in fire_list*/) {
        (x,y,z)=t.get_xyz();
        screen[scx(x)][scy(y)]='F';
        wait=0.2;
    }
    out=[];
    for (/*y in range(sizey)*/) {
        for (/*x in range(sizex)*/) {
            out.append(screen[x][y]);
        }
        out.append(goal_txt.get(y," "*goal_width));
        out.append('\n');
    }
    print string.join(out,'');
#endif
}
