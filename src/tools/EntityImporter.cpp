//
// Copyright (C) 2009 Alistair Riddoch
// Copyright (C) 2013 Erik Ogenvik
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

#include "EntityImporter.h"

#include "common/serialno.h"
#include "common/log.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <fstream>
#include <iostream>
#include <common/debug.h>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Set;
using Atlas::Message::Element;
namespace {
    class ObjectDecoder : public Atlas::Objects::ObjectsDecoder
    {
        private:
            void objectArrived(Atlas::Objects::Root obj) override
            {
                m_check = true;
                m_obj = std::move(obj);
            }

            bool m_check;
            Atlas::Objects::Root m_obj;
        public:
            ObjectDecoder(const Atlas::Objects::Factories& factories) :
                    ObjectsDecoder(factories),
                    m_check(false)
            {
            }

            bool check() const
            {
                return m_check;
            }

            const Atlas::Objects::Root& get()
            {
                m_check = false;
                return m_obj;
            }
    };
}

EntityImporter::EntityImporter(const std::string& accountId,
                               const std::string& avatarId) :
    EntityImporterBase(accountId, avatarId)
{
    EventCompleted.connect(sigc::mem_fun(*this, &EntityImporter::completed));
}

EntityImporter::~EntityImporter() = default;

void EntityImporter::setup(const std::string& arg, OpVector& ret)
{
    mCurrentRes = &ret;
    start(arg);

    mCurrentRes = nullptr;
}

void EntityImporter::operation(const Operation& op, OpVector& res)
{
    mCurrentRes = &res;

//    std::cout << "Got op ==================" << std::endl;
//    debug_dump(op, std::cout);

    if (!op->isDefaultRefno()) {
        auto I = mCallbacks.find(op->getRefno());
        if (I != mCallbacks.end()) {
            auto callback = I->second;
            //Don't erase callbacks, since we can get multiple responses for the same serial number
            callback(op);
        }
    } else {
        if (op->getClassNo() == Atlas::Objects::Operation::ERROR_NO) {
            std::string message =
                op->getArgs().front()->getAttr("message").asString();
            log(ERROR, String::compose("Got error. Message: %1", message));
        }
    }

    mCurrentRes = nullptr;
}

void EntityImporter::completed()
{
    m_complete = true;
}

long int EntityImporter::newSerialNumber()
{
    return newSerialNo();
}

void EntityImporter::send(const Atlas::Objects::Operation::RootOperation& op)
{
    if (mCurrentRes) {
        mCurrentRes->push_back(op);
    }
}

void EntityImporter::sendAndAwaitResponse(
    const Atlas::Objects::Operation::RootOperation& op,
    CallbackFunction& callback)
{
    if (mCurrentRes) {
        mCallbacks.emplace(op->getSerialno(), callback);
        mCurrentRes->push_back(op);
    }
}

Atlas::Objects::Root EntityImporter::loadFromFile(const std::string& filename)
{
    Atlas::Objects::Factories factories;
    std::fstream fileStream(filename, std::ios::in);
    ObjectDecoder atlasLoader(factories);

    Atlas::Codecs::XML codec(fileStream, fileStream, atlasLoader);
    codec.poll();

    return atlasLoader.get();
}
