// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Plant.h"

#include "Script.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/random.h"
#include "common/Property.h"

#include "common/log.h"

#include "common/Tick.h"
#include "common/Chop.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Move.h>

static const bool debug_flag = false;

Plant::Plant(const std::string & id) : Plant_parent(id), m_fruits(0),
                                                         m_radius(1),
                                                         m_fruitChance(2),
                                                         m_sizeAdult(4)
{
    // Default to a 1m cube
    m_location.m_bBox = BBox(WFMath::Point<3>(-0.5, -0.5, 0),
                             WFMath::Point<3>(0.5, 0.5, 1));

    subscribe("chop", OP_CHOP);
    subscribe("tick", OP_TICK);
    subscribe("touch", OP_TOUCH);

    m_properties["fruits"] = new Property<int>(m_fruits, a_fruit);
    m_properties["radius"] = new Property<int>(m_radius, a_fruit);
    m_properties["fruitName"] = new Property<std::string>(m_fruitName, a_fruit);
    m_properties["fruitChance"] = new Property<int>(m_fruitChance, a_fruit);
    m_properties["sizeAdult"] = new Property<double>(m_sizeAdult, a_fruit);
}

Plant::~Plant()
{
}

int Plant::dropFruit(OpVector & res)
{
    if (m_fruits < 1) { return 0; }
    if (m_fruitName.empty()) { return 0; }
    int drop = std::min(m_fruits, randint(m_minuDrop, m_maxuDrop));
    m_fruits -= drop;
    debug(std::cout << "Dropping " << drop << " fruits from "
                    << m_type << " plant." << std::endl << std::flush;);
    float height = m_location.m_bBox.highCorner().z(); 
    for(int i = 0; i < drop; ++i) {
        float rx = m_location.m_pos.x() + uniform( height * m_radius,
                                                  -height * m_radius);
        float ry = m_location.m_pos.y() + uniform( height * m_radius,
                                                  -height * m_radius);
        MapType fmap;
        fmap["name"] = m_fruitName;
        fmap["parents"] = ListType(1,m_fruitName);
        Location floc(m_location.m_loc, Point3D(rx, ry, 0));
        floc.addToMessage(fmap);
        RootOperation * create = new Create();
        create->setTo(getId());
        create->setArgs(ListType(1, fmap));
        res.push_back(create);
    }
    return drop;
}

void Plant::ChopOperation(const Chop & op, OpVector & res)
{
    std::cout << "Plant got chop op" << std::endl << std::flush;
    if (m_script->Operation("tick", op, res)) {
        return;
    }
    Element mode;
    if (get("mode", mode) && mode.isString() && mode.asString() == "felled") {
        std::cout << "Plant is already down" << std::endl << std::flush;
        RootOperation * op = new Set;
        ListType & setArgs = op->getArgs();
        setArgs.push_back(MapType());
        MapType & setArg = setArgs.back().asMap();
        setArg["id"] = getId();
        setArg["status"] = -1;
        op->setTo(getId());
        res.push_back(op);

        if (m_location.m_bBox.isValid()) {
            std::cout << "Plant replaced by log" << std::endl << std::flush;
            op = new Create;
            ListType & createArgs = op->getArgs();
            createArgs.push_back(MapType());
            MapType & createArg = createArgs.back().asMap();
            createArg["parents"] = ListType(1,"lumber");
            m_location.addToMessage(createArg);
            // createArg["mass"] = FIXE What? Calculate?
            op->setTo(getId());
            res.push_back(op);
        }
        return;
    }
    // FIXME In the future it will take more than one chop to chop down a tree.
    RootOperation * move = new Move;
    ListType & moveArgs = move->getArgs();
    moveArgs.push_back(MapType());
    MapType & moveArg = moveArgs.back().asMap();
    if (m_location.m_loc != NULL) {
        moveArg["loc"] = m_location.m_loc->getId();
    } else {
        log(ERROR, "Plant generating invalid Move op because LOC is NULL");
    }
    Vector3D axis(uniform(-1, 1), uniform(-1, 1), 0);
    axis.normalize();
    // FIXME Make tree fall away from axe, by using cross product of
    // distance to axe, and vertical axis as axis of rotation
    Quaternion orient(m_location.m_orientation);
    orient.rotation(axis, M_PI/2);
    moveArg["orientation"] = orient.toAtlas();
    moveArg["mode"] = "felled";
    moveArg["pos"] = m_location.m_pos.toAtlas();
    moveArg["id"] = getId();
    move->setTo(getId());
    res.push_back(move);
}

void Plant::TickOperation(const Tick & op, OpVector & res)
{
    debug(std::cout << "Plant::Tick(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    if (m_script->Operation("tick", op, res)) {
        return;
    }
    RootOperation * tickOp = new Tick;
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
    if ((dropped != 0) || (m_status < 1.)) {
        RootOperation * set = new Set();
        MapType pmap;
        pmap["id"] = getId();
        pmap["fruits"] = m_fruits;
        if (m_status < 1.) {
            // FIXME Very very fast recovery
            double newStatus = m_status + 0.0001;
            pmap["status"] = (newStatus > 1.f) ? 1.f : newStatus;
        }
        set->setTo(getId());
        set->setArgs(ListType(1,pmap));
        res.push_back(set);
    }
}

void Plant::TouchOperation(const Touch & op, OpVector & res)
{
    debug(std::cout << "Plant::Touch(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    debug(std::cout << "Plant has " << m_fruits << " fruits right now"
                    << std::endl << std::flush;);
    if (m_script->Operation("touch", op, res)) {
        return;
    }
    debug(std::cout << "Checking for drop"
                    << std::endl << std::flush;);

    int dropped = dropFruit(res);
    if (dropped != 0) {
        RootOperation * set = new Set();
        MapType pmap;
        pmap["id"] = getId();
        pmap["fruits"] = m_fruits;
        set->setTo(getId());
        set->setArgs(ListType(1,pmap));
        res.push_back(set);
    }
}
