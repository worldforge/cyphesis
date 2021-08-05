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
template<typename T>
struct OpQueEntry
{
    bool operator<(const OpQueEntry& right) const
    {
        if (time_for_dispatch == right.time_for_dispatch) {
            return sequence < right.sequence;
        }
        return time_for_dispatch < right.time_for_dispatch;
    }

    bool operator>(const OpQueEntry& right) const
    {
        if (time_for_dispatch == right.time_for_dispatch) {
            return sequence > right.sequence;
        }
        return time_for_dispatch > right.time_for_dispatch;
    }

    Operation op;
    Ref<T> from;
    std::chrono::milliseconds time_for_dispatch;
    //Sequence number is used to determine ordering when to ops have the exact same time.
    long sequence;

    explicit OpQueEntry(Operation o, T& f, long sequence_);

    OpQueEntry(Operation op_, Ref<T> from_, long sequence_)
            : op(std::move(op_)),
              from(std::move(from_)),
              time_for_dispatch(std::chrono::milliseconds(static_cast<std::int64_t>(op->getSeconds() * 1000))),
              sequence(sequence_)
    {
    }

    OpQueEntry(const OpQueEntry& o);

    OpQueEntry(OpQueEntry&& o) noexcept;

    ~OpQueEntry();

    constexpr OpQueEntry& operator=(OpQueEntry&& rhs) noexcept
    {
        this->op = std::move(rhs.op);
        this->from = std::move(rhs.from);
        this->time_for_dispatch = std::move(rhs.time_for_dispatch);
        this->sequence = std::move(rhs.sequence);
        return *this;
    }

    const Operation& operator*() const
    {
        return op;
    }

    Atlas::Objects::Operation::RootOperationData* operator->() const
    {
        return op.get();
    }


};

struct OperationsHandler
{

    /**
     * Gets time until the next operation needs to be dispatched.
     * @return Seconds.
     */
    virtual std::chrono::steady_clock::duration timeUntilNextOp(const std::chrono::steady_clock::duration& currentTime) const = 0;

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

    virtual void dispatchNextOp() = 0;

    virtual size_t processUntil(std::chrono::steady_clock::duration duration, std::chrono::steady_clock::duration maxWallClockDuration) = 0;
};

/// \brief Handles dispatching of operations at suitable time.
///
template<typename T>
class OperationsDispatcher : public OperationsHandler
{
    public:

        typedef std::function<std::chrono::steady_clock::duration()> TimeProviderFnType;

        /**
         * @brief Ctor.
         * @param operationProcessor A processor function called each time an operation needs to be processed.
         */
        OperationsDispatcher(std::function<void(const Operation&, Ref<T>)> operationProcessor,
                             TimeProviderFnType timeProviderFn);

        virtual ~OperationsDispatcher();

        /**
         * Gets the number of seconds until the next operation needs to be dispatched.
         * @return Seconds.
         */
        std::chrono::steady_clock::duration timeUntilNextOp(const std::chrono::steady_clock::duration& currentTime) const override;

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

        /// \brief Add an operation to the ordered op queue.
        ///
        /// The op *must* have "seconds" set before calling this method.
        void addOperationToQueue(Operation, Ref<T>);

        size_t getQueueSize() const override;


        /**
         * If set to >
         */
        std::chrono::milliseconds m_time_diff_report;

        const std::priority_queue<OpQueEntry<T>, std::vector<OpQueEntry<T>>, std::greater<OpQueEntry<T>>>& getQueue() const
        {
            return m_operationQueue;
        }

        std::priority_queue<OpQueEntry<T>, std::vector<OpQueEntry<T>>, std::greater<OpQueEntry<T>>>& getQueue()
        {
            return m_operationQueue;
        }

        void dispatchNextOp() override;

        size_t processUntil(std::chrono::steady_clock::duration duration, std::chrono::steady_clock::duration maxWallClockDuration) override;

    protected:

        std::function<void(const Operation&, Ref<T>)> m_operationProcessor;
        const TimeProviderFnType m_timeProviderFn;

        /// An ordered queue of operations to be dispatched in the future
        std::priority_queue<OpQueEntry<T>, std::vector<OpQueEntry<T>>, std::greater<OpQueEntry<T>>> m_operationQueue;
        /// Keeps track of if the operation queues are dirty.
        bool m_operation_queues_dirty;

        /// A sequence number, used when ops that have the same second set needs ordering.
        long m_sequence;


        /**
         * @brief Dispatches the operation contained in the OpQueueEntry.
         * @param opQueueEntry An entry from an op queue.
         */
        void dispatchOperation(OpQueEntry<T>& opQueueEntry);

        std::chrono::steady_clock::duration getTime() const;

};


#endif /* OPERATIONSDISPATCHER_H_ */
