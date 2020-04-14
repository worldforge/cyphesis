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

#ifndef CYPHESIS_CYPY_COMMON_H
#define CYPHESIS_CYPY_COMMON_H

#include "pycxx/CXX/Extensions.hxx"

/**
 * \ingroup PythonWrappers
 */
class CyPy_Const : public Py::ExtensionModule<CyPy_Const>
{
    public:
        CyPy_Const();
};


/**
 * \ingroup PythonWrappers
 */
class CyPy_Globals : public Py::ExtensionModule<CyPy_Globals>
{
    public:
        CyPy_Globals();
};


/**
 * \ingroup PythonWrappers
 */
class CyPy_Log : public Py::ExtensionModule<CyPy_Log>
{
    public:
        CyPy_Log();

    private:
        Py::Object debug_(const Py::Tuple& args);

        Py::Object thinking(const Py::Tuple& args);

};

/**
 * \ingroup PythonWrappers
 */
class CyPy_Common : public Py::ExtensionModule<CyPy_Common>
{
    public:
        CyPy_Common();

        static std::string init();

    private:
        CyPy_Log m_log;
        CyPy_Const m_const;
        CyPy_Globals m_globals;

        Py::Object new_serial_no();

};


#endif //CYPHESIS_CYPY_COMMON_H
