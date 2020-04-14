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


#include "CyPy_Common.h"

#include "common/log.h"
#include "common/const.h"
#include "common/globals.h"

#include <wfmath/const.h>
#include <common/serialno.h>

CyPy_Const::CyPy_Const() : ExtensionModule("const")
{
    initialize("Const");

    Py::Dict d(moduleDictionary());
    d["debug_level"] = Py::Long(consts::debug_level);
    d["debug_thinking"] = Py::Long(consts::debug_thinking);
    d["time_multiplier"] = Py::Float(consts::time_multiplier);
    d["basic_tick"] = Py::Float(consts::basic_tick);
    d["epsilon"] = Py::Float(WFMath::numeric_constants<WFMath::CoordType>::epsilon());

}

CyPy_Globals::CyPy_Globals() : ExtensionModule("globals")
{
    initialize("Globals");

    Py::Dict d(moduleDictionary());
    d["share_directory"] = Py::String(share_directory);
}


CyPy_Log::CyPy_Log() : ExtensionModule("log")
{

    add_varargs_method("debug", &CyPy_Log::debug_, "");
    add_varargs_method("thinking", &CyPy_Log::thinking, "");

    initialize("Logging");

}

Py::Object CyPy_Log::debug_(const Py::Tuple& args)
{
    if (consts::debug_level != 0) {
        args.verify_length(2);
        Py::Long level(args[0]);
        auto message = args[1].as_string();

        if (consts::debug_level >= level) {
            log(SCRIPT, message);
        }
    }
    return Py::None();
}

Py::Object CyPy_Log::thinking(const Py::Tuple& args)
{
    if (consts::debug_thinking != 0) {
        args.verify_length(1);
        auto message = args[0].as_string();

        log(SCRIPT, message);
    }

    return Py::None();
}


CyPy_Common::CyPy_Common() : ExtensionModule("common")
{

    add_noargs_method("new_serial_no", &CyPy_Common::new_serial_no, "Gets the next system wide serial number.");

    initialize("Common");

    Py::Dict d(moduleDictionary());
    d["log"] = m_log.moduleObject();
    d["const"] = m_const.moduleObject();
    d["globals"] = m_globals.moduleObject();


}

std::string CyPy_Common::init()
{
    PyImport_AppendInittab("common", []() {
        static auto module = new CyPy_Common();
        return module->module().ptr();
    });
    return "common";
}

Py::Object CyPy_Common::new_serial_no()
{
       return Py::Long(newSerialNo());
}
