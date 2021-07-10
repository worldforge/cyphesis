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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/ExternalMindsManager.h"
#include "server/PossessionAuthenticator.h"

#include "../PropertyCoverage.h"

#include "server/MindProperty.h"

int main()
{
    PossessionAuthenticator possessionAuthenticator;
    ExternalMindsManager mindsManager(possessionAuthenticator);
    MindProperty ap;

    PropertyChecker<MindProperty> pc(ap);

    // Coverage is complete, but it wouldn't hurt to add some bad data here.

    pc.basicCoverage();

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

#include "../TestWorld.h"
#include "rules/ai/BaseMind.h"
#include "rules/python/PythonScriptFactory.h"

#include "../stubs/server/stubExternalMindsManager.h"
#include "../stubs/server/stubExternalMindsConnection.h"
#include "../stubs/server/stubPossessionAuthenticator.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/pythonbase/stubPythonClass.h"

template<>
PythonScriptFactory<BaseMind>::PythonScriptFactory(const std::string & package,
                                                   const std::string & type) :
                                                   PythonClass(package,
                                                               type)
{
}

template <class T>
int PythonScriptFactory<T>::setup()
{
    return 0;
}

template <class T>
const std::string & PythonScriptFactory<T>::package() const
{
    return m_package;
}

template <class T>
int PythonScriptFactory<T>::addScript(T & entity) const
{
    return 0;
}

template <class T>
int PythonScriptFactory<T>::refreshClass()
{
    return 0;
}

template class PythonScriptFactory<BaseMind>;

int GetScriptDetails(const Atlas::Message::MapType & script,
                     const std::string & class_name,
                     const std::string & context,
                     std::string & script_package,
                     std::string & script_class)
{
    return 0;
}
