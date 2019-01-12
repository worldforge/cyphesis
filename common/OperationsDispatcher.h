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
#include "const.h"

#include <Atlas/Objects/RootOperation.h>

#include <list>
#include <set>
#include <queue>
#include <functional>
#include "modules/Ref.h"

#include <chrono>

/// \brief Type to hold an operation and the Entity it is from   for efficiency
/// when broadcasting.
template <typename T>
struct OpQueEntry {
    bool operator<(const OpQueEntry& right) const {
        return op->getSeconds() < right->getSeconds();
    }

    bool operator>(const OpQueEntry& right) const {
        return op->getSeconds() > right->getSeconds();
    }
    Operation op;
    Ref<T> from;

    explicit OpQueEntry(Operation o, T & f);
    OpQueEntry(Operation op, Ref<T> from) : op(std::move(op)), from(std::move(from))
    {
    }
    OpQueEntry(const OpQueEntry & o);
    OpQueEntry(OpQueEntry && o) noexcept;
    ~OpQueEntry();

    constexpr OpQueEntry& operator=(OpQueEntry&& rhs) noexcept
    {
        this->op = std::move(rhs.op);
        this->from = std::move(rhs.from);
        return *this;
    }

    const Operation & operator*() const {
        return op;
    }

    Atlas::Objects::Operation::RootOperationData * operator->() const {
        return op.get();
    }


};

struct OperationsHandler {
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
    virtual bool idle(int numberOfOpsToProcess) = 0;

    /**
     * Gets time until the next operation needs to be dispatched.
     * @return Seconds.
     */
    virtual std::chrono::microseconds timeUntilNextOp() const = 0;

    /**
     * @brief Checks if the operation queues have been marked as dirty.
     *
     * This means that something has been added to them.
     * @return True if the queues are dirty.
     */
    virtual bool isQueueDirty() const = 0;

    /**
     * @brief Marks all queues as clean.
     */
    virtual void markQueueAsClean() = 0;

    /**
     * @brief Removes all operations from the queues.
     */
    virtual void clearQueues() = 0;

    virtual size_t getQueueSize() const = 0;
};
/// \brief Handles dispatching of operations at suitable time.
///
template <typename T>
class OperationsDispatcher : public OperationsHandler
{
    public:
        /**
         * @brief Ctor.
         * @param operationProcessor A processor function called each time an operation needs to be processed.
         */
        OperationsDispatcher(const std::function<void(const Operation&, Ref<T>)>& operationProcessor, const std::function<double()>& timeProviderFn);

        virtual ~OperationsDispatcher();

        /// \brief Main world loop function.
        /// This function is called whenever the communications code is idle.
        /// It updates the in-game time, and dispatches operations that are
        /// now due for dispatch. The number of operations dispatched is limited
        /// to a supplied number to ensure that client communications are always handled
        /// in a timely manner. If the maximum number of operations are dispatched, the return
        /// value indicates that this is the case, and the communications code
        /// will call this function again as soon as possible rather than sleeping.
        /// This ensures that the maximum possible number of operations are dispatched
        /// without becoming unresponsive to client communications traffic.
        bool idle(int numberOfOpsToProcess) override;

        /**
         * Gets the number of seconds until the next operation needs to be dispatched.
         * @return Seconds.
         */
        std::chrono::microseconds timeUntilNextOp() const override;

        /**
         * @brief Checks if the operation queues have been marked as dirty.
         *
         * This means that something has been added to them.
         * @return True if the queues are dirty.
         */
        bool isQueueDirty() const override;

        /**
         * @brief Marks all queues as clean.
         */
        void markQueueAsClean() override;

        /**
         * @brief Removes all operations from the queues.
         */
        void clearQueues() override;

        /**
         * @brief Adds an operation to the queue.
         *
         * This will increase the reference count of the supplied LocatedEntity until the operation is handled.
         *
         * @param The operation to add.
         * @param The located entity it belongs to.
         */
        void addOperationToQueue(Operation, Ref<T>);

        size_t getQueueSize() const override;


        /**
         * If set to >
         */
        float m_time_diff_report;

    protected:

        std::function<void(const Operation&, Ref<T>)> m_operationProcessor;
        const std::function<double()> m_timeProviderFn;

        /// An ordered queue of operations to be dispatched in the future
        std::priority_queue<OpQueEntry<T>, std::vector<OpQueEntry<T>>, std::greater<OpQueEntry<T>> > m_operationQueue;
        /// Keeps track of if the operation queues are dirty.
        bool m_operation_queues_dirty;


        /**
         * @brief Dispatches the operation contained in the OpQueueEntry.
         * @param opQueueEntry An entry from an op queue.
         */
        void dispatchOperation(OpQueEntry<T>& opQueueEntry);

        double getTime() const;

};




#endif /* OPERATIONSDISPATCHER_H_ */
