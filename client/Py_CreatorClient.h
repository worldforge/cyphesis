// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef CLIENT_PY_CREATORCLIENT_H
#define CLIENT_PY_CREATORCLIENT_H

#include <Python.h>

#include <string>

class CreatorClient;

extern PyTypeObject PyCreatorClient_Type;

/// \brief Wrapper for CreatorClient in Python
typedef struct {
    PyObject_HEAD
    PyObject         * CreatorClient_attr;    // Attributes dictionary
    CreatorClient    * m_mind;
} PyCreatorClient;

#define PyCreatorClient_Check(_o) ((PyTypeObject*)PyObject_Type((PyObject*)_o)==&PyCreatorClient_Type)

PyCreatorClient * newPyCreatorClient();
int runClientScript(CreatorClient *, const std::string &, const std::string &);

#endif // CLIENT_PY_CREATORCLIENT_H
