/*
 Copyright (C) 2013 Erik Ogenvik

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

#ifndef ENTITYTRAVERSALTASK_H_
#define ENTITYTRAVERSALTASK_H_

#include "common/ClientTask.h"
#include <Atlas/Objects/Entity.h>

#include <functional>
#include <string>

/**
 * \brief A client task for traversing the entity tree.
 *
 * A function is supplied to the task. When active the task will
 * walk through the entity hierarchy on the server, calling the supplied
 * function for every entity it receives, until either all entities have been
 * visited or it's told to stop.
 */
class EntityTraversalTask: public ClientTask
{
    public:

        /**
         * \brief Ctor.
         *
         * A function must be supplied. The signature of the function is
         * bool(const RootEntity&)
         * The supplied entity is a representation of an entity on the server.
         * If the function returns true if the traversal should continue.
         * \param accountId An account id.
         * \param visitor A visitor function.
         */
        EntityTraversalTask(const std::string& accountId,
                std::function<bool(const Atlas::Objects::Entity::RootEntity&)>& visitor);

        ~EntityTraversalTask() override;

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
                std::vector<std::string> children;
                /**
                 * @brief The current children iterator. This is an iterator of the "children" field.
                 */
                std::vector<std::string>::const_iterator currentChildIterator;
        };

        /**
         * \brief A logged in account id.
         */
        const std::string mAccountId;

        /**
         * \brief A visitor function applied to each entity.
         */
        std::function<bool(const Atlas::Objects::Entity::RootEntity&)> mVisitor;

        /**
         * \brief The last serial number used.
         */
        long int mSerial;

        /**
         * \brief A stack keeping track of the traversal.
         */
        std::list<StackEntry> mStack;

        /**
         * \brief Requests a new entity from the server.
         * \param id The id of the entity.
         * \param res
         */
        void getEntity(const std::string & id, OpVector & res);

};

#endif /* ENTITYTRAVERSALTASK_H_ */
