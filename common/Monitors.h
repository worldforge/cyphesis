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

// $Id: 40e6d5dd3bbd85dc591e034a2da9f6f63522e267 $

#ifndef COMMON_MONITORS_H
#define COMMON_MONITORS_H

#include <Atlas/Message/Element.h>

/// \brief Base class for dynamic variable monitors
///
/// Variables which should be represented in the monitors output are
/// referenced by subclasses this interface
class MonitorBase {
  public:
    virtual ~MonitorBase() = 0;
    virtual void send(std::ostream &) = 0;
};

template<typename T>
class Monitor : public MonitorBase {
  protected:
    const T & m_variable;
  public:
    Monitor(const T & variable);

    virtual ~Monitor();
    virtual void send(std::ostream &);
};

/// \brief Storage for monitor values to be exported
///
/// Any code can insert or update key value pairs here, and subsystems like
/// the http interface can access it.
class Monitors {
  protected:
    typedef std::map<std::string, MonitorBase *> MonitorDict;

    static Monitors * m_instance;

    Monitors();

    Atlas::Message::MapType m_pairs;
    MonitorDict m_variableMonitors;
  public:
    static Monitors * instance();
    static void cleanup();

    void insert(const std::string &, const Atlas::Message::Element &);
    void watch(const std::string &, MonitorBase *);
    void send(std::ostream &);
};

#endif // COMMON_DATABSE_H
