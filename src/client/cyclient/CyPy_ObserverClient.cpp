/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "rules/python/CyPy_Operation.h"
#include "rules/python/CyPy_Oplist.h"
#include "CyPy_ObserverClient.h"
#include "CyPy_CreatorClient.h"

CyPy_ObserverClient::CyPy_ObserverClient(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    throw Py::RuntimeError("Can't be created from Python.");

//    m_value = new ObserverClient();
}

CyPy_ObserverClient::CyPy_ObserverClient(Py::PythonClassInstance* self, ObserverClient* value)
    : WrapperBase(self, value)
{

}

void CyPy_ObserverClient::init_type()
{
    behaviors().name("ObserverClient");
    behaviors().doc("");

    PYCXX_ADD_VARARGS_METHOD(setup, setup, "");
    PYCXX_ADD_VARARGS_METHOD(create_avatar, create_avatar, "");
    PYCXX_ADD_VARARGS_METHOD(send, send, "");
    PYCXX_ADD_VARARGS_METHOD(send_wait, send_wait, "");
    PYCXX_ADD_NOARGS_METHOD(teardown, teardown, "");
    PYCXX_ADD_NOARGS_METHOD(run, run, "");
    PYCXX_ADD_NOARGS_METHOD(wait, wait, "");

    behaviors().readyType();
}

Py::Object CyPy_ObserverClient::getattro(const Py::String& name)
{
    auto nameStr = name.as_string();
    if (nameStr == "character") {
        if (!m_value->character()) {
            return Py::None();
        }
        return CyPy_CreatorClient::wrap(m_value->character());
    }
    if (nameStr == "id") {
        return Py::String(m_value->id());
    }

    return PythonExtensionBase::getattro(name);
}

int CyPy_ObserverClient::setattro(const Py::String& name, const Py::Object& attr)
{
    auto nameStr = name.as_string();

    if (nameStr == "server") {
        m_value->setServer(verifyString(attr));
        return 0;
    }
    return PythonExtensionBase::setattro(name, attr);
}


Py::Object CyPy_ObserverClient::setup(const Py::Tuple& args)
{
    std::string username;
    std::string password;
    std::string avatar;

    if (args.length() == 1) {
        throw Py::IndexError("function takes 0 or 2, or 3 arguments (1 given)");
    } else if (args.length() > 3) {
        throw Py::IndexError("function takes 0 or 2, or 3 arguments (>3 given)");
    } else if (args.length() >= 2) {
        username = verifyString(args[0]);
        password = verifyString(args[1]);
    } else if (args.length() == 3) {
        avatar = verifyString(args[2]);
    }
    int res;
    if (avatar.empty()) {
        res = m_value->setup(username, password);
    } else {
        res = m_value->setup(username, password, avatar);
    }
    if (res != 0) {
        throw Py::RuntimeError("client setup failed");
    }
    return Py::None();
}

Py::Object CyPy_ObserverClient::teardown()
{
    m_value->teardown();
    return Py::None();
}

Py::Object CyPy_ObserverClient::create_avatar(const Py::Tuple& args)
{
    args.verify_length(1);
    auto c = m_value->createCharacter(verifyString(args.front()));
    if (!c) {
        throw Py::RuntimeError("avatar creation failed");
    }
    return CyPy_CreatorClient::wrap(std::move(c));
}

Py::Object CyPy_ObserverClient::run()
{
    return Py::None();
}

Py::Object CyPy_ObserverClient::send(const Py::Tuple& args)
{
    args.verify_length(1);
    m_value->send(CyPy_Operation::value(args.front()));
    return Py::None();
}

Py::Object CyPy_ObserverClient::send_wait(const Py::Tuple& args)
{
    args.verify_length(1);
    auto resOp = m_value->sendAndWaitReply(CyPy_Operation::value(args.front()));
    //FIXME: We should always return an Oplist
    if (!resOp.isValid()) {
        return Py::None();
    } else  {
        return CyPy_Operation::wrap(std::move(resOp));
    }
//    else {
//        return CyPy_Oplist::wrap(std::move(res));
//    }
}

Py::Object CyPy_ObserverClient::wait()
{
    int ret = m_value->wait();
    if (ret != 0) {
        throw Py::RuntimeError("Timeout waiting for reply");
    }
    return Py::None();
}
