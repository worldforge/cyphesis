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


#ifndef COMMON_MONITORS_H
#define COMMON_MONITORS_H

#include <Atlas/Message/Element.h>
#include "Singleton.h"
#include <memory>

class VariableBase;

/// \brief Storage for monitor values to be exported
///
/// Any code can insert or update key value pairs here, and subsystems like
/// the http interface can access it.
class Monitors : public Singleton<Monitors>{
  protected:
    typedef std::map<std::string, std::unique_ptr<VariableBase>> MonitorDict;



    Atlas::Message::MapType m_pairs;
    MonitorDict m_variableMonitors;
  public:
    Monitors();
    ~Monitors() override;

    void insert(const std::string &, const Atlas::Message::Element &);
    void watch(const std::string &, VariableBase *);
    void send(std::ostream &);
    void sendNumerics(std::ostream &);
    int readVariable(const std::string& key, std::ostream& out_stream) const;

};

#endif // COMMON_MONITORS_H
