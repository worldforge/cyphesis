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


#ifndef COMMON_CLIENT_TASK_H
#define COMMON_CLIENT_TASK_H

#include "common/OperationRouter.h"

#include <string>
#include <functional>

/// \brief Base class for admin tasks which run for some time.
///
/// Typical tasks that inherit from this class are ones which last for
/// non trivial time and will typically require the user to be able to
/// continue issuing commands.
class ClientTask {
  protected:
    /// \brief Flag that indicates when the task is complete
    bool m_complete;
    /// \brief Description of the task running
    std::string m_description;

    ///\brief ClientTask constructor
    ClientTask();
  public:
    virtual ~ClientTask();

    /// \brief Set up the task processing user arguments
    virtual void setup(const std::string & arg, OpVector &) = 0;
    /// \brief Handle an operation from the server
    virtual void operation(const Operation &, OpVector &) = 0;

    /// \brief Check whether the task is complete
    ///
    /// @return true if the task is complete, false otherwise
    bool isComplete() const { return m_complete; }

    /// \brief Accessor for task description
    const std::string & description() { return m_description; }
};

class FunctionClientTask : public ClientTask {
    protected:
        std::function<bool(const Operation &, OpVector &)> m_function;
    public:
        explicit FunctionClientTask(std::function<bool(const Operation &, OpVector &)> function);
        /// \brief Set up the task processing user arguments
        void setup(const std::string & arg, OpVector &) override {}
        /// \brief Handle an operation from the server
        void operation(const Operation &, OpVector &) override;
};
#endif // COMMON_CLIENT_TASK_H
