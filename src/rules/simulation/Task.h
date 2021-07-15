// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004-2005 Alistair Riddoch
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


#ifndef RULESETS_TASK_H
#define RULESETS_TASK_H

#include "common/OperationRouter.h"
#include "rules/simulation/UsageInstance.h"
#include "pycxx/CXX/Objects.hxx"

#include <Atlas/Message/Element.h>

#include <string>

#include <cassert>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include "modules/ReferenceCounted.h"

class LocatedEntity;

class Script;

struct TaskUsage
{
    std::string name;
    std::map<std::string, UsageParameter> params;
};

/// \brief Interface class for handling tasks which take a short while to
/// complete.
/// If you create an instance of this though Python you must be careful to call "irrelevant"
/// when shutting down it. This is to make sure it releases its script reference. If not there's
/// a risk of having a cyclic relationship between Python objects, with the effect of memory not being reclaimed.
class Task : public boost::noncopyable, public ReferenceCounted
{
    protected:

        /// \brief Serial number of the tick due to arrive next at this task.
        int m_serialno;

        /// \brief Flag to indicate if this task is obsolete and should be removed
        bool m_obsolete;

        /// \brief Progress towards task completion
        double m_progress;

        /// \brief Rate of progress towards task completion
        double m_rate;

        double m_start_time;

        /// \brief Additional task attributes
        Atlas::Message::MapType m_attr;

        /// \brief Name of task presented to client
        std::string m_name;

        /// \brief The language script that will handle this task
        Py::Object m_script;

        /**
         * The usages that are attached to this task, which clients can invoke.
         */
        std::vector<TaskUsage> m_usages;

        std::string m_action;

    public:

        static std::function<Py::Object(const std::map<std::string, std::vector<UsageParameter::UsageArg>>& args)> argsCreator;

        boost::optional<double> m_duration;

        boost::optional<double> m_tick_interval;

        UsageInstance m_usageInstance;

        /// \brief Constructor
        explicit Task(UsageInstance usageInstance, Py::Object script);

        ~Task() override;

        void callUsageScriptFunction(const std::string& function, const std::map<std::string, std::vector<UsageParameter::UsageArg>>& args, OpVector& res);

        void callScriptFunction(const std::string& function, const Py::Tuple& args, OpVector& res);

        /// \brief Flag this task as obsolete
        void irrelevant();

        /// \brief Handle the operation that instigates the task
        ///
        /// @param res The result of the operation is returned here.
        void initTask(const std::string& id, OpVector& res);

        /// \brief Handle an operation to perform the task
        ///
        /// A Task gets regular ticks which cause whatever actions this
        /// Task involves to be returned.
        /// @param res The result of the operation is returned here.
        /// @return True if the task was changed.
        bool tick(const std::string& id, const Operation& op, OpVector& res);

        /// \brief Create a new tick op for the next iteration of this task
        Operation nextTick(const std::string& id, const Operation& op);

        /// \brief Return the number of the next to arrive at this task
        int serialno() const
        {
            return m_serialno;
        }

        /// \brief Return a new tick serial number.
        int newTick()
        {
            return ++m_serialno;
        }

        /// \brief Accessor to determine if this Task is obsolete
        bool obsolete()
        { return m_obsolete; }

        /// \brief Accessor for name of this task
        std::string& name()
        { return m_name; }

        /// \brief Accessor for progress towards completion
        double& progress()
        { return m_progress; }

        /// \brief Accessor for rate of progress towards completion
        double& rate()
        { return m_rate; }

        std::vector<TaskUsage>& usages()
        { return m_usages; }

        /// \brief Accessor for additional attributes
        int getAttr(const std::string& attr, Atlas::Message::Element& val) const;

        /// \brief Sets additional attribute
        void setAttr(const std::string& attr, const Atlas::Message::Element& val);

        void startAction(std::string actionName, OpVector& res);

        void stopAction(OpVector& res);

        friend class Tasktest;
};

#endif // RULESETS_TASK_H
