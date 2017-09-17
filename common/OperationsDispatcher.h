/*
 Copyright (C) 2014 Erik Ogenvik

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

#ifndef OPERATIONSDISPATCHER_H_
#define OPERATIONSDISPATCHER_H_

#include "OperationRouter.h"

#include <Atlas/Objects/RootOperation.h>

#include <list>
#include <set>
#include <queue>
#include <functional>

class LocatedEntity;

/// \brief Type to hold an operation and the Entity it is from   for efficiency
/// when broadcasting.
struct OpQueEntry {
    bool operator<(const OpQueEntry& right) const {
        return op->getSeconds() < right->getSeconds();
    }

    bool operator>(const OpQueEntry& right) const {
        return op->getSeconds() > right->getSeconds();
    }
    Operation op;
    LocatedEntity* from;

    explicit OpQueEntry(const Operation & o, LocatedEntity & f);
    OpQueEntry(const OpQueEntry & o);
    ~OpQueEntry();

    const Operation & operator*() const {
        return op;
    }

    Atlas::Objects::Operation::RootOperationData * operator->() const {
        return op.get();
    }


};

typedef std::queue<OpQueEntry> OpQueue;
typedef std::priority_queue<OpQueEntry, std::vector<OpQueEntry>, std::greater<OpQueEntry> > OpPriorityQueue;

/// \brief Handles dispatching of operations at suitable time.
///
class OperationsDispatcher
{
    public:
        /**
         * @brief Ctor.
         * @param operationProcessor A processor function called each time an operation needs to be processed.
         */
        OperationsDispatcher(const std::function<void(const Operation&, LocatedEntity&)>& operationProcessor, const std::function<double()>& timeProviderFn);

        virtual ~OperationsDispatcher();

        /// \brief Main world loop function.
        /// This function is called whenever the communications code is idle.
        /// It updates the in-game time, and dispatches operations that are
        /// now due for dispatch. The number of operations dispatched is limited
        /// to 10 to ensure that client communications are always handled in a timely
        /// manner. If the maximum number of operations are dispatched, the return
        /// value indicates that this is the case, and the communications code
        /// will call this function again as soon as possible rather than sleeping.
        /// This ensures that the maximum possible number of operations are dispatched
        /// without becoming unresponsive to client communications traffic.
        bool idle();

        /**
         * Gets the number of seconds until the next operation needs to be dispatched.
         * @return Seconds.
         */
        double secondsUntilNextOp() const;

        /**
         * @brief Checks if the operation queues have been marked as dirty.
         *
         * This means that something has been added to them.
         * @return True if the queues are dirty.
         */
        bool isQueueDirty() const;

        /**
         * @brief Marks all queues as clean.
         */
        void markQueueAsClean();

        /**
         * @brief Removes all operations from the queues.
         */
        void clearQueues();

        /**
         * @brief Adds an operation to the queue.
         *
         * This will increase the reference count of the supplied LocatedEntity until the operation is handled.
         *
         * @param The operation to add.
         * @param The located entity it belongs to.
         */
        void addOperationToQueue(const Operation &,
                        LocatedEntity &);
    protected:

        std::function<void(const Operation&, LocatedEntity&)> m_operationProcessor;
        const std::function<double()> m_timeProviderFn;

        /// An ordered queue of operations to be dispatched in the future
        OpPriorityQueue m_operationQueue;
        /// Keeps track of if the operation queues are dirty.
        bool m_operation_queues_dirty;

        /**
         * @brief Dispatches the operation contained in the OpQueueEntry.
         * @param opQueueEntry An entry from an op queue.
         */
        void dispatchOperation(const OpQueEntry& opQueueEntry);

        double getTime() const;

};

#endif /* OPERATIONSDISPATCHER_H_ */
