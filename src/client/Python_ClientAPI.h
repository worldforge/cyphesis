// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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


#ifndef CLIENT_PYTHON_CLIENT_API_H
#define CLIENT_PYTHON_CLIENT_API_H

#include <map>
#include <string>

class ObserverClient;

void extend_client_python_api();
void python_prompt();
int python_client_script(const std::string &,
                         const std::string &,
                         ObserverClient &);

#endif // CLIENT_PYTHON_CLIENT_API_H
