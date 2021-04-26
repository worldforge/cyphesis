//
// Copyright (C) 2009 Alistair Riddoch
// Copyright (C) 2012 Erik Ogenvik
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

#include "EntityExporter.h"


#include "common/serialno.h"
#include "common/log.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/MultiLineListFormatter.h>

#include <chrono>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Talk;
using Atlas::Objects::Operation::Get;
using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

EntityExporter::EntityExporter(const std::string& accountId,
                               const std::string& avatarId) :
        EntityExporterBase(accountId, avatarId, getCurrentTime()), mCurrentRes(
        nullptr)
{
    assert(accountId != "");
    assert(avatarId != "");
}

void EntityExporter::setup(const std::string& arg, OpVector& ret)
{
    mCurrentRes = &ret;

    //Start by asking the server for information about itself
    //so we can provide correct meta data in fillWithServerData()
    Get get;
    get->setObjtype("op");
    get->setSerialno(newSerialNumber());
    CallbackFunction callback = sigc::mem_fun(*this,
                                              &EntityExporter::operationGetServerInfo);
    sendAndAwaitResponse(get, callback);

    start(arg, "0");

    mCurrentRes = nullptr;

    if (mComplete || mCancelled) {
        m_complete = true;
    }
}

void EntityExporter::operation(const Operation& op, OpVector& res)
{
    mCurrentRes = &res;

    if (!op->isDefaultRefno()) {
        auto I = mCallbacks.find(op->getRefno());
        if (I != mCallbacks.end()) {
            auto callback = I->second;
            mCallbacks.erase(I);
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

    if (mComplete || mCancelled) {
        m_complete = true;
    }
}

long int EntityExporter::newSerialNumber()
{
    return newSerialNo();
}

void EntityExporter::send(const Atlas::Objects::Operation::RootOperation& op)
{
    if (mCurrentRes) {
        mCurrentRes->push_back(op);
    }
}

void EntityExporter::sendAndAwaitResponse(
        const Atlas::Objects::Operation::RootOperation& op,
        CallbackFunction& callback)
{
    if (mCurrentRes) {
        mCallbacks.emplace(op->getSerialno(), callback);
        mCurrentRes->push_back(op);
    }
}

Atlas::Formatter* EntityExporter::createMultiLineFormatter(std::iostream& s,
                                                           Atlas::Bridge& b)
{
    return new Atlas::MultiLineListFormatter(s, b);
}

std::string EntityExporter::getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    std::stringstream ss;
    ss << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return ss.str();
}

void EntityExporter::fillWithServerData(Atlas::Message::MapType& serverMap)
{
    Element e;
    if (m_server_info->copyAttr("ruleset", e) == 0 && e.isString()) {
        serverMap["ruleset"] = e.asString();
    }
    if (m_server_info->copyAttr("version", e) == 0 && e.isString()) {
        serverMap["version"] = e.asString();
    }
}

void EntityExporter::operationGetServerInfo(
        const Atlas::Objects::Operation::RootOperation& op)
{
    if (!op->getArgs().empty()) {
        m_server_info = op->getArgs().front();
    }

}

