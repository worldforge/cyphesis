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

struct Sink : public Atlas::Bridge
{
    /**
     * Set to true whenever we've gotten data.
     */
    bool got_data = false;

    ~Sink() override = default;


    void streamBegin() override
    {};

    void streamMessage() override
    {};

    void streamEnd() override
    {};

    void mapMapItem(std::string name) override
    { got_data = true; };

    void mapListItem(std::string name) override
    { got_data = true; };

    void mapIntItem(std::string name, Atlas::Message::IntType) override
    { got_data = true; };

    void mapFloatItem(std::string name, Atlas::Message::FloatType) override
    { got_data = true; };

    void mapStringItem(std::string name, std::string) override
    { got_data = true; };

    void mapNoneItem(std::string name) override
    { got_data = true; };

    void mapEnd() override
    { got_data = true; };

    void listMapItem() override
    { got_data = true; };

    void listListItem() override
    { got_data = true; };

    void listIntItem(Atlas::Message::IntType) override
    { got_data = true; };

    void listFloatItem(Atlas::Message::FloatType) override
    { got_data = true; };

    void listStringItem(std::string) override
    { got_data = true; };

    void listNoneItem() override
    { got_data = true; };

    void listEnd() override
    { got_data = true; };
};

#endif // TESTS_SINK_H
