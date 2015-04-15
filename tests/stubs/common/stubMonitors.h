// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#include "common/Monitors.h"


using Atlas::Message::Element;
using Atlas::Message::MapType;

Monitors * Monitors::m_instance = NULL;

Monitors::Monitors()
{
}

Monitors::~Monitors()
{

}

Monitors * Monitors::instance()
{
    if (m_instance == NULL) {
        m_instance = new Monitors();
    }
    return m_instance;
}

void Monitors::cleanup()
{
    delete m_instance;

    m_instance = 0;
}

void Monitors::insert(const std::string & key, const Element & val)
{
}

void Monitors::watch(const::std::string & name, VariableBase * monitor)
{
}

void Monitors::send(std::ostream & io)
{
}

void Monitors::sendNumerics(std::ostream & io)
{
}

int Monitors::readVariable(const std::string& key, std::ostream& out_stream) const
{
    return 1;
}
