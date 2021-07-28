/*
 Copyright (C) 2018 Erik Ogenvik

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

#include "RepeatedTask.h"
#include "Remotery.h"

RepeatedTask::RepeatedTask(boost::asio::io_context& io_context, std::chrono::steady_clock::duration interval, std::function<void()> function)
    : mInterval(std::move(interval)),
      mTimer(io_context),
      mFunction(std::move(function))
{
#if BOOST_VERSION >= 106600
    mTimer.expires_after(mInterval);
#else
    mTimer.expires_from_now(mInterval);
#endif
    mTimer.async_wait([this](boost::system::error_code ec) {
        if (!ec) {
            this->executeTask();
        }
    });
}

void RepeatedTask::cancel()
{
    mTimer.cancel();
}

void RepeatedTask::executeTask()
{
    rmt_ScopedCPUSample(RepeatedTaskExecute, 0)
    mFunction();
#if BOOST_VERSION >= 106600
    mTimer.expires_after(mInterval);
#else
    mTimer.expires_from_now(mInterval);
#endif
    mTimer.async_wait([this](boost::system::error_code ec) {
        if (!ec) {
            this->executeTask();
        }
    });
}
