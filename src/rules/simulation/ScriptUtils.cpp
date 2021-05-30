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

#include "ScriptUtils.h"
#include "rules/python/CyPy_LocatedEntity.h"
#include "rules/python/CyPy_Operation.h"
#include "rules/python/CyPy_Oplist.h"
#include "common/debug.h"

static const bool debug_flag = false;


HandlerResult ScriptUtils::processScriptResult(const std::string& scriptName, const Py::Object& ret, OpVector& res, LocatedEntity& e)
{
    HandlerResult result = OPERATION_IGNORED;

    auto processPythonResultFn = [&](const Py::Object& pythonResult) {
        if (pythonResult.isLong()) {
            auto numRet = Py::Long(pythonResult).as_long();
            if (numRet == 0) {
                result = OPERATION_IGNORED;
            } else if (numRet == 1) {
                result = OPERATION_HANDLED;
            } else if (numRet == 2) {
                result = OPERATION_BLOCKED;
            } else {
                log(ERROR, String::compose("Unrecognized return code %1 for script '%2' attached to entity '%3'", numRet, scriptName, e.describeEntity()));
            }

        } else if (CyPy_Operation::check(pythonResult)) {
            auto operation = CyPy_Operation::value(pythonResult);
            assert(operation);
            //If nothing is set the operation is from the entity containing the usages.
            if (operation->isDefaultFrom()) {
                operation->setFrom(e.getId());
            }
            res.push_back(std::move(operation));
        } else if (CyPy_Oplist::check(pythonResult)) {
            auto& o = CyPy_Oplist::value(pythonResult);
            for (auto& opRes : o) {
                //If nothing is set the operation is from the entity containing the usages.
                if (opRes->isDefaultFrom()) {
                    opRes->setFrom(e.getId());
                }
                res.push_back(opRes);
            }
        } else {
            log(ERROR, String::compose("Python script \"%1\" returned an invalid "
                                       "result.", scriptName));
        }
    };

    if (ret.isNone()) {
        debug_print("Returned none")
    } else {
        //Check if it's a tuple and process it.
        if (ret.isTuple()) {
            for (auto item : Py::Tuple(ret)) {
                processPythonResultFn(item);
            }
        } else {
            processPythonResultFn(ret);
        }
    }

    return result;
}
