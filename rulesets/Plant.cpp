// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Plant.h"

#include "Script.h"

#include "common/const.h"
#include "common/random.h"

#include "common/Tick.h"

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Set.h>

Plant::Plant(const std::string & id) : Plant_parent(id), m_fruits(0),
                                                         m_radius(1),
                                                         m_fruitName("seed")
{
    // Default to a 1m cube
    m_location.m_bBox = BBox(Vector3D(-0.5, -0.5, 0), Vector3D(0.5, 0.5, 1));

    subscribe("tick", OP_TICK);
}

Plant::~Plant()
{
}

bool Plant::get(const std::string & aname, Element & attr) const
{
    if (aname == "fruits") {
        attr = m_fruits;
        return true;
    } else if (aname == "radius") {
        attr = m_radius;
        return true;
    } else if (aname == "fruitName") {
        attr = m_fruitName;
        return true;
    } else if (aname == "fruitChance") {
        attr = m_fruitChance;
        return true;
    } else if (aname == "sizeAdult") {
        attr = m_sizeAdult;
        return true;
    }
    return Plant_parent::get(aname, attr);
}

void Plant::set(const std::string & aname, const Element & attr)
{
    if ((aname == "fruits") && attr.IsInt()) {
        m_fruits = attr.AsInt();
        m_update_flags |= a_fruit;
    } else if ((aname == "radius") && attr.IsInt()) {
        m_radius = attr.AsInt();
    } else if ((aname == "fruitName") && attr.IsString()) {
        m_fruitName = attr.AsString();
    } else if ((aname == "fruitChance") && attr.IsInt()) {
        m_fruitChance = attr.AsInt();
    } else if ((aname == "sizeAdult") && attr.IsNum()) {
        m_sizeAdult = attr.AsNum();
    } else {
        Plant_parent::set(aname, attr);
    }
}

void Plant::addToObject(Element::MapType & omap) const
{
    omap["fruits"] = m_fruits;
    omap["radius"] = m_radius;
    omap["fruitName"] = m_fruitName;
    omap["fruitChance"] = m_fruitChance;
    omap["sizeAdult"] = m_sizeAdult;
    Plant_parent::addToObject(omap);
}

int Plant::dropFruit(OpVector & res)
{
    if (m_fruits < 1) { return 0; }
    int drop = std::min(m_fruits, randint(m_minuDrop, m_maxuDrop));
    m_fruits = m_fruits - drop;
    double height = m_location.m_bBox.farPoint().Z(); 
    for(int i = 0; i < drop; i++) {
        double rx = m_location.m_pos.X()+uniform(height*m_radius, -height*m_radius);
        double ry = m_location.m_pos.X()+uniform(height*m_radius, -height*m_radius);
        Element::MapType fmap;
        fmap["name"] = m_fruitName;
        fmap["parents"] = Element::ListType(1,m_fruitName);
        Location floc(m_location.m_loc, Vector3D(rx, ry, 0));
        floc.addToObject(fmap);
        RootOperation * create = new Create(Create::Instantiate());
        create->SetArgs(Element::ListType(1, fmap));
        res.push_back(create);
    }
    return drop;
}

OpVector Plant::TickOperation(const Tick & op)
{
    OpVector res;
    m_script->Operation("tick", op, res);
    RootOperation * tickOp = new Tick(Tick::Instantiate());
    tickOp->SetTo(getId());
    tickOp->SetFutureSeconds(consts::basic_tick * m_speed);
    res.push_back(tickOp);
    int dropped = dropFruit(res);
    if (m_location.m_bBox.farPoint().Z() > m_sizeAdult) {
        if (randint(1, m_fruitChance) == 1) {
            m_fruits++;
            dropped--;
        }
    }
    if (dropped != 0) {
        RootOperation * set = new Set(Set::Instantiate());
        Element::MapType pmap;
        pmap["id"] = getId();
        pmap["fruits"] = m_fruits;
        set->SetArgs(Element::ListType(1,pmap));
        res.push_back(set);
    }
    return res;
}
