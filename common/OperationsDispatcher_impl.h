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

static const bool opdispatcher_debug_flag = false;

template <typename T>
OperationsDispatcher<T>::~OperationsDispatcher()
{
    clearQueues();
}


template <typename T>
void OperationsDispatcher<T>::dispatchOperation(OpQueEntry<T> & oqe)
{
    //Set the time of when this op is dispatched. That way, other components in the system can
    //always use the seconds set on the op to know the current time.
    oqe.op->setSeconds(getTime());
    try {
        m_operationProcessor(oqe.op, std::move(oqe.from));
    }
    catch (const std::exception & ex) {
        log(ERROR, String::compose("Exception caught in WorldRouter::idle() "
                                   "thrown while processing operation "
                                   "sent to \"%1\" from \"%2\": %3",
                                   oqe->getTo(), oqe->getFrom(), ex.what()));
    }
    catch (...) {
        log(ERROR, String::compose("Unspecified exception caught in WorldRouter::idle() "
                                   "thrown while processing operation "
                                   "sent to \"%1\" from \"%2\"",
                                   oqe->getTo(), oqe->getFrom()));
    }
}


template <typename T>
bool OperationsDispatcher<T>::idle(int numberOfOpsToProcess)
{
    int op_count = 0;

    double realtime = getTime();
    bool opsAvailableRightNow = !m_operationQueue.empty() && m_operationQueue.top()->getSeconds() <= realtime;

    while (opsAvailableRightNow && op_count < numberOfOpsToProcess) {
        ++op_count;
        auto opQueueEntry = m_operationQueue.top();
        //Pop it before we dispatch it, since dispatching might alter the queue.
        m_operationQueue.pop();
        dispatchOperation(opQueueEntry);

        opsAvailableRightNow = !m_operationQueue.empty() && m_operationQueue.top()->getSeconds() <= realtime;
    };
    // If there are still ops to deliver return true
    // to tell the server not to sleep when polling clients. This ensures
    // that we keep processing ops at a the maximum rate without leaving
    // clients unattended.
    Monitors::instance().insert("operations_queue", (Atlas::Message::IntType) m_operationQueue.size());
    return opsAvailableRightNow;
}


template <typename T>
bool OperationsDispatcher<T>::isQueueDirty() const
{
    return m_operation_queues_dirty;
}

template <typename T>
void OperationsDispatcher<T>::markQueueAsClean()
{
    m_operation_queues_dirty = false;
}

template <typename T>
double OperationsDispatcher<T>::getTime() const
{
    return m_timeProviderFn();
}

template <typename T>
double OperationsDispatcher<T>::secondsUntilNextOp() const
{
    if (m_operationQueue.empty()) {
        //600 is a fairly large number of seconds
        return 600.0;
    }
    return m_operationQueue.top()->getSeconds() - getTime();
}



template <typename T>
OpQueEntry<T>::OpQueEntry(Operation o, T & f) :
    op(std::move(o)),
    from(&f)
{
}

template <typename T>
OpQueEntry<T>::OpQueEntry(const OpQueEntry & o) :
    op(o.op),
    from(o.from)
{
}

template<typename T>
OpQueEntry<T>::OpQueEntry(OpQueEntry&& o) noexcept
    : op(std::move(o.op)),
    from(std::move(o.from))
{

}

template <typename T>
OpQueEntry<T>::~OpQueEntry() = default;



template <typename T>
OperationsDispatcher<T>::OperationsDispatcher(const std::function<void(const Operation &, Ref<T>)> & operationProcessor,
                                           const std::function<double()> & timeProviderFn)
    : m_operationProcessor(operationProcessor),
      m_timeProviderFn(timeProviderFn),
      m_operation_queues_dirty(false)
{
}

template <typename T>
void OperationsDispatcher<T>::clearQueues()
{
    m_operationQueue = std::priority_queue<OpQueEntry<T>, std::vector<OpQueEntry<T>>, std::greater<OpQueEntry<T>>>();
}

/// \brief Add an operation to the ordered op queue.
///
/// Any time adjustment required is made to the operation, and it
/// is added to the appropriate place in the chronologically ordered
/// queue. The From attribute of the operation is set to the id of
/// the entity that is responsible for adding the operation to the
/// queue.
template <typename T>
void OperationsDispatcher<T>::addOperationToQueue(Operation op, Ref<T> ent)
{
    assert(op.isValid());
    assert(op->getFrom() != "cheat");

    m_operation_queues_dirty = true;
    op->setFrom(ent->getId());
    if (!op->hasAttrFlag(Atlas::Objects::Operation::SECONDS_FLAG)) {
        if (!op->hasAttrFlag(Atlas::Objects::Operation::FUTURE_SECONDS_FLAG)) {
            op->setSeconds(getTime());
        } else {
            double t = getTime() + (op->getFutureSeconds() * consts::time_multiplier);
            op->setSeconds(t);
            op->removeAttrFlag(Atlas::Objects::Operation::FUTURE_SECONDS_FLAG);
        }
    }
    m_operationQueue.emplace(std::move(op), std::move(ent));
    if (opdispatcher_debug_flag) {
        std::cout << "WorldRouter::addOperationToQueue {" << std::endl;
        debug_dump(op, std::cout);
        std::cout << "}" << std::endl << std::flush;
    }
}


#endif /* OPERATIONSDISPATCHER_IMPL_H_ */
