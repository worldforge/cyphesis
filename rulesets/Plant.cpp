// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Plant.h"

#include "Script.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/random.h"

#include "common/Tick.h"

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Touch.h>

static const bool debug_flag = false;

Plant::Plant(const std::string & id) : Plant_parent(id), m_fruits(0),
                                                         m_radius(1),
                                                         m_fruitChance(2),
                                                         m_sizeAdult(4),
                                                         m_fruitName("seed")
{
    // Default to a 1m cube
    m_location.m_bBox = BBox(WFMath::Point<3>(-0.5, -0.5, 0),
                             WFMath::Point<3>(0.5, 0.5, 1));

    subscribe("tick", OP_TICK);
    subscribe("touch", OP_TOUCH);
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
    if ((aname == "fruits") && attr.isInt()) {
        m_fruits = attr.asInt();
        m_update_flags |= a_fruit;
    } else if ((aname == "radius") && attr.isInt()) {
        m_radius = attr.asInt();
    } else if ((aname == "fruitName") && attr.isString()) {
        m_fruitName = attr.asString();
    } else if ((aname == "fruitChance") && attr.isInt()) {
        m_fruitChance = attr.asInt();
    } else if ((aname == "sizeAdult") && attr.isNum()) {
        m_sizeAdult = attr.asNum();
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
    m_fruits -= drop;
    debug(std::cout << "Dropping " << drop << " fruits from "
                    << m_type << " plant." << std::endl << std::flush;);
    float height = m_location.m_bBox.highCorner().z(); 
    for(int i = 0; i < drop; i++) {
        float rx = m_location.m_pos.x() + uniform( height * m_radius,
                                                  -height * m_radius);
        float ry = m_location.m_pos.y() + uniform( height * m_radius,
                                                  -height * m_radius);
        Element::MapType fmap;
        fmap["name"] = m_fruitName;
        fmap["parents"] = Element::ListType(1,m_fruitName);
        Location floc(m_location.m_loc, Vector3D(rx, ry, 0));
        floc.addToObject(fmap);
        RootOperation * create = new Create();
        create->setTo(getId());
        create->setArgs(Element::ListType(1, fmap));
        res.push_back(create);
    }
    return drop;
}

OpVector Plant::TickOperation(const Tick & op)
{
    debug(std::cout << "Plant::Tick(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    OpVector res;
    if (m_script->Operation("tick", op, res)) {
        return res;
    }
    RootOperation * tickOp = new Tick();
    tickOp->setTo(getId());
    tickOp->setFutureSeconds(consts::basic_tick * m_speed);
    res.push_back(tickOp);
    int dropped = dropFruit(res);
    if (m_location.m_bBox.isValid() && 
        (m_location.m_bBox.highCorner().z() > m_sizeAdult)) {
        if (randint(1, m_fruitChance) == 1) {
            m_fruits++;
            dropped--;
        }
    }
    if (dropped != 0) {
        RootOperation * set = new Set();
        Element::MapType pmap;
        pmap["id"] = getId();
        pmap["fruits"] = m_fruits;
        set->setTo(getId());
        set->setArgs(Element::ListType(1,pmap));
        res.push_back(set);
    }
    return res;
}

OpVector Plant::TouchOperation(const Touch & op)
{
    debug(std::cout << "Plant::Touch(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    debug(std::cout << "Plant has " << m_fruits << " fruits right now"
                    << std::endl << std::flush;);
    OpVector res;
    if (m_script->Operation("touch", op, res)) {
        return res;
    }
    debug(std::cout << "Checking for drop"
                    << std::endl << std::flush;);

    int dropped = dropFruit(res);
    if (dropped != 0) {
        RootOperation * set = new Set();
        Element::MapType pmap;
        pmap["id"] = getId();
        pmap["fruits"] = m_fruits;
        set->setTo(getId());
        set->setArgs(Element::ListType(1,pmap));
        res.push_back(set);
    }
    return res;
}
