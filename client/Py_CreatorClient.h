// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000 Alistair Riddoch
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


#ifndef CLIENT_PY_CREATOR_CLIENT_H
#define CLIENT_PY_CREATOR_CLIENT_H

#include <Python.h>

#include <string>

class CreatorClient;
class CharacterClient;

extern PyTypeObject PyCreatorClient_Type;

/// \brief Wrapper for CreatorClient in Python
typedef struct {
    PyObject_HEAD
    /// \brief CreatorClient object handled by this wrapper
    union {
        CharacterClient * c;
        CreatorClient * a;
    } m_mind;
} PyCreatorClient;

#define PyCreatorClient_Check(_o) (Py_TYPE((_o)) == &PyCreatorClient_Type)

PyCreatorClient * newPyCreatorClient();

#endif // CLIENT_PY_CREATOR_CLIENT_H
