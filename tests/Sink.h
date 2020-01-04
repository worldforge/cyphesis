// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifndef TESTS_SINK_H
#define TESTS_SINK_H

#include <Atlas/Bridge.h>
#include <Atlas/Message/Element.h>

class Sink : public Atlas::Bridge
{
  public:
    ~Sink() override = default;


    void streamBegin() override { };
    void streamMessage() override { };
    void streamEnd() override { };

    void mapMapItem(std::string name) override { };
    void mapListItem(std::string name) override { };
    void mapIntItem(std::string name, Atlas::Message::IntType) override { };
    void mapFloatItem(std::string name, Atlas::Message::FloatType) override { };
    void mapStringItem(std::string name, std::string) override { };
    void mapNoneItem(std::string name) override {};
    void mapEnd() override { };

    void listMapItem() override { };
    void listListItem() override { };
    void listIntItem(Atlas::Message::IntType) override { };
    void listFloatItem(Atlas::Message::FloatType) override { };
    void listStringItem(std::string) override { };
    void listNoneItem() override {};
    void listEnd() override { };
};

#endif // TESTS_SINK_H
