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


#ifndef TOOLS_OPERATION_MONITOR_H
#define TOOLS_OPERATION_MONITOR_H

#include "common/ClientTask.h"
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Bridge.h>
#include <memory>
#include <fstream>

/// \brief Task class for monitoring all in-game operations occuring.
class OperationMonitor : public ClientTask
{
    protected:

        Atlas::Message::QueuedDecoder mDecoder;
        std::unique_ptr<Atlas::Bridge> mCodec;
        std::unique_ptr<Atlas::Objects::ObjectsEncoder> mEncoder;
        std::unique_ptr<std::ostream> mOutFile;
        std::ostream* mOutStream;

        int op_count;
    public:
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

        ~OperationMonitor() override;

        int count() const
        {
            return op_count;
        }


        void setup(const std::string& arg, OpVector&) override;

        void operation(const Operation& op, OpVector&) override;
};

#endif // TOOLS_OPERATION_MONITOR_H
