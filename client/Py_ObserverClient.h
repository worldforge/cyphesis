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


#ifndef CLIENT_PY_OBSERVER_CLIENT_H
#define CLIENT_PY_OBSERVER_CLIENT_H

#include <Python.h>

#include <string>

class ObserverClient;

extern PyTypeObject PyObserverClient_Type;

/// \brief Wrapper for ObserverClient in Python
typedef struct {
    PyObject_HEAD
    /// \brief CreateClient object handled by this wrapper
    ObserverClient    * m_client;
} PyObserverClient;

#define PyObserverClient_Check(_o) ((_o)->ob_type == &PyObserverClient_Type)

PyObserverClient * newPyObserverClient();

#endif // CLIENT_PY_OBSERVER_CLIENT_H
