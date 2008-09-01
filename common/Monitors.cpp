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

// $Id$

#include "Monitors.h"

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;

MonitorBase::~MonitorBase()
{
}

template <typename T>
Monitor<T>::Monitor(const T & variable) : m_variable(variable)
{
}

template <typename T>
Monitor<T>::~Monitor()
{
}

template <typename T>
void Monitor<T>::send(std::ostream & o)
{
    o << m_variable;
}

template class Monitor<int>;
template class Monitor<std::string>;
template class Monitor<const char *>;

Monitors * Monitors::m_instance = NULL;

Monitors::Monitors()
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
    if (m_instance != 0) {
        delete m_instance;
    }

    m_instance = 0;
}

void Monitors::insert(const std::string & key, const Element & val)
{
    m_pairs[key] = val;
}

void Monitors::watch(const::std::string & name, MonitorBase * monitor)
{
    m_variableMonitors[name] = monitor;
}

static std::ostream & operator<<(std::ostream & s, const Element & e)
{
    switch (e.getType()) {
      case Atlas::Message::Element::TYPE_INT:
        s << e.Int();
        break;
      case Atlas::Message::Element::TYPE_FLOAT:
        s << e.Float();
        break;
      case Atlas::Message::Element::TYPE_STRING:
        s << e.String();
        break;
      default:
        break;
    };
    return s;
}

void Monitors::send(std::ostream & io)
{
    MapType::const_iterator I = m_pairs.begin();
    MapType::const_iterator Iend = m_pairs.end();
    for (; I != Iend; ++I) {
        io << I->first << " " << I->second << std::endl;
    }

    MonitorDict::const_iterator J = m_variableMonitors.begin();
    MonitorDict::const_iterator Jend = m_variableMonitors.end();
    for (; J != Jend; ++J) {
        io << J->first << " ";
        J->second->send(io);
        io << std::endl;
    }
}
