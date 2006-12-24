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

// $Id: Py_CreatorClient.h,v 1.11 2006-12-24 17:18:55 alriddoch Exp $

#ifndef CLIENT_PY_CREATORCLIENT_H
#define CLIENT_PY_CREATORCLIENT_H

#include <Python.h>

#include <string>

class CreatorClient;

extern PyTypeObject PyCreatorClient_Type;

/// \brief Wrapper for CreatorClient in Python
typedef struct {
    PyObject_HEAD
    /// \brief Storage for additional attributes
    PyObject         * CreatorClient_attr;    // Attributes dictionary
    /// \brief CreateClient object handled by this wrapper
    CreatorClient    * m_mind;
} PyCreatorClient;

#define PyCreatorClient_Check(_o) ((_o)->ob_type == &PyCreatorClient_Type)

PyCreatorClient * newPyCreatorClient();
int runClientScript(CreatorClient *, const std::string &, const std::string &);

#endif // CLIENT_PY_CREATORCLIENT_H
