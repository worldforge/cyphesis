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

class Sink : public Atlas::Bridge
{
  public:
    virtual ~Sink() { }

    virtual void streamBegin() { }
    virtual void streamMessage() { }
    virtual void streamEnd() { }
    virtual void mapMapItem(const std::string& name) { }
    virtual void mapListItem(const std::string& name) { }
    virtual void mapIntItem(const std::string& name, long) { }
    virtual void mapFloatItem(const std::string& name, double) { }
    virtual void mapStringItem(const std::string& name, const std::string&) { }
    virtual void mapEnd() { }
    virtual void listMapItem() { }
    virtual void listListItem() { }
    virtual void listIntItem(long) { }
    virtual void listFloatItem(double) { }
    virtual void listStringItem(const std::string&) { }
    virtual void listEnd() { }
};

#endif // TESTS_SINK_H
