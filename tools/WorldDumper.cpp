// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include "WorldDumper.h"

#include "tools/MultiLineListFormatter.h"

#include "common/compose.hpp"
#include "common/id.h"

#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>

#include <iostream>
#include <functional>
#include <cstdlib>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Get;

void WorldDumper::dumpEntity(const RootEntity & ent)
{
    m_encoder->streamObjectsMessage(ent);
}

bool idSorter(const std::string& lhs, const std::string& rhs)
{
  return integerId(lhs) < integerId(rhs);
}


void WorldDumper::infoArrived(const Operation & op, OpVector & res)
{
    if (op->isDefaultRefno() || op->getRefno() != m_lastSerialNo) {
        std::cout << "NOT OURS" << std::endl << std::flush;
        return;
    }
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }
    RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid()) {
        std::cout << "Malformed OURS" << std::endl << std::flush;
        return;
    }
    std::cout << "GOT INFO" << std::endl << std::flush;
    ++m_count;
    //Make a copy so that we can sort the contains list and update it in the
    //entity
    RootEntity entityCopy(ent->copy());
    std::list<std::string> contains = ent->getContains();
    //Sort the contains list so it's deterministic
    contains.sort(idSorter);
    entityCopy->setContains(contains);
    dumpEntity(entityCopy);
    std::list<std::string>::const_iterator I = contains.begin();
    std::list<std::string>::const_iterator Iend = contains.end();
    for (; I != Iend; ++I) {
        m_queue.push_back(*I);
    }


    if (m_queue.empty()) {
        m_formatter->streamBegin();
        m_complete = true;
        std::cout << "COUNTED: " << m_count << std::endl << std::flush;
        return;
    }

    Get get;

    Anonymous get_arg;
    get_arg->setObjtype("obj");
    get_arg->setId(m_queue.front());
    get->setArgs1(get_arg);
    
    get->setFrom(m_account);
    ++m_lastSerialNo;
    get->setSerialno(m_lastSerialNo);
    res.push_back(get);

    m_queue.pop_front();
}

WorldDumper::WorldDumper(const std::string & accountId) : m_account(accountId),
                                                          m_lastSerialNo(-1),
                                                          m_count(0),
                                                          m_codec(0),
                                                          m_encoder(0),
                                                          m_formatter(0)
{
}

WorldDumper::~WorldDumper()
{
    if (m_encoder != 0) {
        delete m_encoder;
    }
    if (m_formatter != 0) {
        delete m_formatter;
    }
    if (m_codec != 0) {
        delete m_codec;
    }
}

void WorldDumper::setup(const std::string & arg, OpVector & res)
{
    std::string filename("world.xml");
    if (!arg.empty()) {
        filename = arg;
    }
    m_file.open(filename.c_str(), std::ios::out);

    m_description = String::compose("dumping %1", filename);

    Atlas::Message::QueuedDecoder * decoder = new Atlas::Message::QueuedDecoder;
    m_codec = new Atlas::Codecs::XML(m_file, *decoder);
    m_formatter = new MultiLineListFormatter(m_file, *m_codec);
    m_encoder = new Atlas::Objects::ObjectsEncoder (*m_formatter);

    m_formatter->streamBegin();

    // Send a get for the root object
    Get get;

    Anonymous get_arg;
    get_arg->setObjtype("obj");
    get_arg->setId("0");
    get->setArgs1(get_arg);
    
    get->setFrom(m_account);
    ++m_lastSerialNo;
    get->setSerialno(m_lastSerialNo);
    res.push_back(get);
}

void WorldDumper::operation(const Operation & op, OpVector & res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        infoArrived(op, res);
    }
}
