/*
 Copyright (C) 2015 Erik Ogenvik

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
#ifndef COMMON_RULETRAVERSALTASK_H_
#define COMMON_RULETRAVERSALTASK_H_

#include "common/ClientTask.h"
#include <Atlas/Objects/Entity.h>

#include <functional>
#include <string>

/**
 * \brief A client task for traversing the rule tree.
 *
 * A function is supplied to the task. When active the task will
 * walk through the rule hierarchy on the server, calling the supplied
 * function for every rule it receives, until either all rules have been
 * visited or it's told to stop.
 */
class RuleTraversalTask : public ClientTask
{
    public:

        /**
         * \brief Ctor.
         *
         * A function must be supplied. The signature of the function is
         * bool(const Root&)
         * The supplied entity is a representation of an rule on the server.
         * If the function returns true if the traversal should continue.
         * \param accountId An account id.
         * \param visitor A visitor function.
         */
        RuleTraversalTask(std::string accountId,
                std::function<bool(const Atlas::Objects::Root&)>& visitor);
        ~RuleTraversalTask() override;

        /// \brief Set up the task processing user arguments
        void setup(const std::string & arg, OpVector &) override;
        /// \brief Handle an operation from the server
        void operation(const Operation &, OpVector &) override;

    private:

          /**
           * \brief Represents one entry on the traversal stack.
           */
          struct StackEntry
          {
                  /**
                   * @brief The ids of the children of the rule.
                   */
                  std::list<std::string> children;
                  /**
                   * @brief The current children iterator. This is an iterator of the "children" field.
                   */
                  std::list<std::string>::const_iterator currentChildIterator;
          };

          /**
           * \brief A logged in account id.
           */
          const std::string mAccountId;

          /**
           * \brief A visitor function applied to each rule.
           */
          std::function<bool(const Atlas::Objects::Root&)> mVisitor;

          /**
           * \brief The last serial number used.
           */
          long int mSerial;

          /**
           * \brief A stack keeping track of the traversal.
           */
          std::list<StackEntry> mStack;

          /**
           * \brief Requests a new rule from the server.
           * \param id The id of the rule.
           * \param res
           */
          void getRule(const std::string & id, OpVector & res);
};

#endif /* COMMON_RULETRAVERSALTASK_H_ */
