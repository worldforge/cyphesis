// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#include "PythonWrapper.h"
#include "CyPy_Operation.h"
#include "CyPy_Oplist.h"
#include "CyPy_LocatedEntity.h"

#include "common/compose.hpp"
#include "common/log.h"
#include "common/operations/Tick.h"
#include "common/debug.h"
#include "pythonbase/Python_API.h"
#include "Remotery.h"
#include <boost/algorithm/string.hpp>

static const bool debug_flag = false;

/// \brief PythonWrapper constructor
PythonWrapper::PythonWrapper(const Py::Object& wrapper)
        : m_wrapper(wrapper)
{
}

PythonWrapper::~PythonWrapper()
{
    for (auto& connection : m_propertyUpdateConnections) {
        connection.disconnect();
    }
}


HandlerResult PythonWrapper::operation(const std::string& op_type,
                                       const Operation& op,
                                       OpVector& res)
{
    rmt_ScopedCPUSample(Python_operation, 0)

    assert(!m_wrapper.isNull());
    std::string op_name = op_type + "_operation";
    debug_print("Got script " << this->m_wrapper.type().str() << " on object " << this->m_wrapper.str() << " for " << op_name);
    if (!m_wrapper.hasAttr(op_name)) {
        debug_print("No method to be found for " << op_name);
        return OPERATION_IGNORED;
    }

    try {
        PythonLogGuard logGuard([this, op_type]() {
            return String::compose("%1, %2: ", this->m_wrapper.str(), op_type);
        });
        auto ret = m_wrapper.callMemberFunction(op_name, Py::TupleN(CyPy_Operation::wrap(op)));

        debug_print("Called python method " << op_name);
        return processScriptResult(op_name, ret, res);

    } catch (const Py::BaseException& py_ex) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        //It's important that we only access m_wrapper.as_string() after PyErr_Print(), as it's invalid to access any Python stuff when there's an error set.
        log(ERROR, String::compose("Python error calling \"%1\" on " +
                                   m_wrapper.as_string(), op_name));
        if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
            log(ERROR,
                String::compose("Script for %1 has reported an error "
                                "processing a tick operation. "
                                "This entity is probably now inactive.",
                                m_wrapper.as_string()));
        }
        return OPERATION_IGNORED;
    }
}

void PythonWrapper::attachPropertyCallbacks(LocatedEntity& entity)
{
    auto list = m_wrapper.dir();
    for (int i = 0; i < list.size(); ++i) {
        auto fieldName = list[i].str().as_string();
        //Look for fields named "<something>_property_update". These are methods that should be called when that property changes.
        if (boost::algorithm::ends_with(fieldName, "_property_update")) {
            auto propertyName = fieldName.substr(0, fieldName.length() - 16);
            auto connection = entity.propertyApplied.connect([this, fieldName, propertyName, &entity](const std::string& changedPropertyName, const PropertyBase& property) {
                if (propertyName == changedPropertyName) {
                    try {
                        PythonLogGuard logGuard([this, fieldName]() {
                            return String::compose("%1, %2: ", this->m_wrapper.str(), fieldName);
                        });
                        OpVector res;
                        auto ret = m_wrapper.callMemberFunction(fieldName);
                        //Ignore Handler result; it does nothing in this context. But process any ops.
                        processScriptResult(fieldName, ret, res);
                        for (auto& resOp: res) {
                            if (resOp->getClassNo() == Atlas::Objects::Operation::SET_NO && !resOp->isDefaultTo() && resOp->getTo() == entity.getId()) {
                                //Handle any Set ops to the own entity directly here, so Set ops that affect multiple properties become atomic.
                                //TODO: how to make sure there's no endless loops here when different properties affect each others?
                                if (!resOp->getArgs().empty()) {
                                    entity.merge(resOp->getArgs().front()->asMessage());
                                }
                            } else {
                                entity.sendWorld(resOp);
                            }
                        }
                    } catch (const Py::BaseException& py_ex) {
                        log(ERROR, String::compose("Could not call property update function %1 on %2 for entity %3", fieldName, m_wrapper.str(), entity.describeEntity()));
                        if (PyErr_Occurred()) {
                            PyErr_Print();
                        }
                    }
                }
            });
            m_propertyUpdateConnections.emplace_back(connection);
        }
    }
}


void PythonWrapper::hook(const std::string& function,
                         LocatedEntity* entity,
                         OpVector& res)
{
    auto wrapper = CyPy_LocatedEntity::wrap(entity);
    if (wrapper.isNull()) {
        return;
    }

    try {
        PythonLogGuard logGuard([this]() {
            return String::compose("%1: ", this->m_wrapper.str());
        });
        auto ret = m_wrapper.callMemberFunction(function, Py::TupleN(wrapper));
        //Ignore Handler result; it does nothing in this context. But process any ops.
        processScriptResult(function, ret, res);
    } catch (const Py::BaseException& py_ex) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        //It's important that we only access m_wrapper.str() after PyErr_Print(), as it's invalid to access any Python stuff when there's an error set.
        log(ERROR, String::compose("Could not call hook function %1 on %2 for entity %3", function, m_wrapper.str(), entity->describeEntity()));
    }

}


HandlerResult PythonWrapper::processScriptResult(const std::string& scriptName, const Py::Object& ret, OpVector& res)
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
                log(ERROR, String::compose("Unrecognized return code %1 for script '%2'", numRet, scriptName));
            }

        } else if (CyPy_Operation::check(pythonResult)) {
            res.push_back(std::move(CyPy_Operation::value(pythonResult)));
        } else if (CyPy_Oplist::check(pythonResult)) {
            auto& o = CyPy_Oplist::value(pythonResult);
            for (auto& opRes : o) {
                res.push_back(opRes);
            }
        } else {
            log(ERROR, String::compose("Python script \"%1\" returned an invalid result.", scriptName));
        }
    };

    if (ret.isNone()) {
        debug_print("Returned none");
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
