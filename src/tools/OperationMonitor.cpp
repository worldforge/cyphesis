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


#include "OperationMonitor.h"

#include <Atlas/Objects/RootOperation.h>

#include <iostream>
#include <memory>
#include <boost/algorithm/string/split.hpp>
#include <Atlas/PresentationBridge.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Codecs/Bach.h>
#include <boost/algorithm/string/classification.hpp>

OperationMonitor::~OperationMonitor() = default;

void OperationMonitor::setup(const std::string& arg, OpVector&)
{
    std::vector<std::string> args;
    boost::algorithm::split(args, arg, boost::algorithm::is_any_of(","));


    std::map<std::string, std::string> options;

    for (auto& anArg : args) {
        std::vector<std::string> pair;
        boost::algorithm::split(pair, anArg, boost::algorithm::is_any_of("="));
        if (pair.size() == 2) {
            auto key = pair.front();
            auto value = *(pair.begin() + 1);
            options.emplace(key, value);
        }
    }


    mOutStream = &std::cout;
    auto I = options.find("file");
    if (I != options.end()) {
        mOutFile = std::make_unique<std::ofstream>(I->second);
        mOutStream = mOutFile.get();
    }
    mCodec = std::make_unique<Atlas::PresentationBridge>(*mOutStream);
    I = options.find("codec");
    if (I != options.end()) {
        if (I->second == "xml") {
            mCodec = std::make_unique<Atlas::Codecs::XML>(std::cin, *mOutStream, mDecoder);
        } else if (I->second == "bach") {
            mCodec = std::make_unique<Atlas::Codecs::Bach>(std::cin, *mOutStream, mDecoder);
        } else if (I->second == "presentation") {
            mCodec = std::make_unique<Atlas::PresentationBridge>(*mOutStream);
        }
    }

    mEncoder = std::make_unique<Atlas::Objects::ObjectsEncoder>(*mCodec);
    startTime = std::chrono::steady_clock::now();

    op_count = 0;

    m_description = "monitoring";
}

void OperationMonitor::operation(const Operation& op, OpVector&)
{
    ++op_count;
    mEncoder->streamObjectsMessage(op);
    *mOutStream << std::endl;

//    std::cout << op->getParent() << "(from=\"" << op->getFrom()
//              << "\",to=\"" << op->getTo() << "\")"
//              << std::endl << std::flush;
}
