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

#ifndef OPERATIONSDISPATCHER_IMPL_H_
#define OPERATIONSDISPATCHER_IMPL_H_

#include "OperationsDispatcher.h"
#include "rules/LocatedEntity.h"
#include "const.h"
#include "debug.h"
#include "Monitors.h"

#include <iostream>
#include <cstdint>
#include <chrono>


static const bool opdispatcher_debug_flag = false;

template<typename T>
OperationsDispatcher<T>::~OperationsDispatcher()
{
    m_operationQueue = decltype(m_operationQueue)();
}


template<typename T>
void OperationsDispatcher<T>::dispatchOperation(OpQueEntry<T>& oqe)
{
    m_operationProcessor(oqe.op, std::move(oqe.from));
}

template<typename T>
void OperationsDispatcher<T>::dispatchNextOp()
{
    if (!m_operationQueue.empty()) {
        auto opQueueEntry = std::move(m_operationQueue.top());
        //Pop it before we dispatch it, since dispatching might alter the queue.
        m_operationQueue.pop();

        dispatchOperation(opQueueEntry);
    }
}

template<typename T>
size_t OperationsDispatcher<T>::processUntil(std::chrono::steady_clock::duration duration, std::chrono::steady_clock::duration maxWallClockDuration)
{
    size_t count = 0;

    auto processUntilWallClock = std::chrono::steady_clock::now() + maxWallClockDuration;
    bool opsAvailableRightNow;
    //Use a "do"-loop to make sure that we at least process one op, even if the wall clock limit doesn't allow for it.
    //This means that in a heavy contested situation we will process one op at least.
    do {
        opsAvailableRightNow = !m_operationQueue.empty() && m_operationQueue.top().time_for_dispatch <= duration;

        if (opsAvailableRightNow) {
            auto opQueueEntry = std::move(m_operationQueue.top());
            //Pop it before we dispatch it, since dispatching might alter the queue.
            m_operationQueue.pop();
            count++;

            if (m_time_diff_report.count() > 0) {
                //Check if there's too large a difference in time
                auto timeDiff = duration - opQueueEntry.time_for_dispatch;
                if (timeDiff > m_time_diff_report) {
                    log(WARNING, String::compose("Op (%1, from %2 to %3) was handled too late. Time diff: %4 seconds. Ops in queue: %5",
                                                 opQueueEntry->getParent(), opQueueEntry.from->describeEntity(),
                                                 opQueueEntry->getTo(), std::chrono::duration_cast<std::chrono::duration<float>>(timeDiff).count(), m_operationQueue.size()));
                }
            }
            dispatchOperation(opQueueEntry);
        }

    } while (opsAvailableRightNow && std::chrono::steady_clock::now() < processUntilWallClock);
    Monitors::instance().insert("operations_queue", (Atlas::Message::IntType) m_operationQueue.size());
    return count;
}


template<typename T>
bool OperationsDispatcher<T>::isQueueDirty() const
{
    return m_operation_queues_dirty;
}

template<typename T>
void OperationsDispatcher<T>::markQueueAsClean()
{
    m_operation_queues_dirty = false;
}

template<typename T>
std::chrono::steady_clock::duration OperationsDispatcher<T>::getTime() const
{
    return m_timeProviderFn();
}

template<typename T>
std::chrono::steady_clock::duration OperationsDispatcher<T>::timeUntilNextOp(const std::chrono::steady_clock::duration& currentTime) const
{
    if (m_operationQueue.empty()) {
        //600 is a fairly large number of seconds
        return std::chrono::seconds(600);
    }
    return m_operationQueue.top().time_for_dispatch - currentTime;
}


template<typename T>
OpQueEntry<T>::OpQueEntry(Operation o, T& f, long sequence_) :
        op(std::move(o)),
        from(&f),
        time_for_dispatch(std::chrono::milliseconds(static_cast<std::int64_t>(op->getSeconds() * 1000))),
        sequence(sequence_)
{
}

template<typename T>
OpQueEntry<T>::OpQueEntry(const OpQueEntry& o) :
        op(o.op),
        from(o.from),
        time_for_dispatch(std::chrono::milliseconds(static_cast<std::int64_t>(op->getSeconds() * 1000))),
        sequence(o.sequence)
{
}

template<typename T>
OpQueEntry<T>::OpQueEntry(OpQueEntry&& o) noexcept
        : op(std::move(o.op)),
          from(std::move(o.from)),
          time_for_dispatch(std::chrono::milliseconds(static_cast<std::int64_t>(op->getSeconds() * 1000))),
          sequence(std::move(o.sequence))
{

}

template<typename T>
OpQueEntry<T>::~OpQueEntry() = default;


template<typename T>
OperationsDispatcher<T>::OperationsDispatcher(std::function<void(const Operation&, Ref<T>)> operationProcessor,
                                              TimeProviderFnType timeProviderFn)
        :       m_time_diff_report(0),
                m_operationProcessor(std::move(operationProcessor)),
                m_timeProviderFn(std::move(timeProviderFn)),
                m_operation_queues_dirty(false),
                m_sequence(0)
{
}

template<typename T>
void OperationsDispatcher<T>::clearQueues()
{
    m_operationQueue = decltype(m_operationQueue)();
}


template<typename T>
void OperationsDispatcher<T>::addOperationToQueue(Operation op, Ref<T> ent)
{
    assert(op.isValid());
    assert(!op->isDefaultSeconds());

    //Check the sequence number of the first op at start.
    long topSequenceNr = 0;
    if (!m_operationQueue.empty()) {
        topSequenceNr = m_operationQueue.top().sequence;
    }
    op->setFrom(ent->getId());
    if (opdispatcher_debug_flag) {
        std::cout << "OperationsDispatcher::addOperationToQueue {" << std::endl;
        debug_dump(op, std::cout);
        std::cout << "}" << std::endl << std::flush;
    }
    m_operationQueue.emplace(std::move(op), std::move(ent), ++m_sequence);
    //Only mark the queue as dirty if the first entry has changed.
    if (topSequenceNr != m_operationQueue.top().sequence) {
        m_operation_queues_dirty = true;
    }
}

template<typename T>
size_t OperationsDispatcher<T>::getQueueSize() const
{
    return m_operationQueue.size();
}


#endif /* OPERATIONSDISPATCHER_IMPL_H_ */
