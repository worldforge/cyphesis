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


#include "common/OperationsDispatcher.h"

OpQueEntry::OpQueEntry(const Operation & o, LocatedEntity & f) : op(o),
                                                                        from(&f)
{
}

OpQueEntry::OpQueEntry(const OpQueEntry & o) : op(o.op), from(o.from)
{
}

OpQueEntry::~OpQueEntry()
{
}


OperationsDispatcher::OperationsDispatcher(const std::function<void(const Operation&, LocatedEntity&)>& operationProcessor, const std::function<double()>& timeProviderFn)
: m_operationProcessor(operationProcessor), m_timeProviderFn(timeProviderFn)
{
}

OperationsDispatcher::~OperationsDispatcher()
{
}

void OperationsDispatcher::clearQueues()
{
}

void OperationsDispatcher::dispatchOperation(const OpQueEntry& oqe)
{
}

void OperationsDispatcher::addOperationToQueue(const Operation & op, LocatedEntity & ent)
{

}

bool OperationsDispatcher::idle()
{
    return false;
}


bool OperationsDispatcher::isQueueDirty() const
{
    return false;
}

void OperationsDispatcher::markQueueAsClean()
{
}

double OperationsDispatcher::getTime() const
{
    return 0.0f;
}

double OperationsDispatcher::secondsUntilNextOp() const {
    return 0.0f;
}

