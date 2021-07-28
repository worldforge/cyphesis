#include <memory>

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

#include "FileSystemObserver.h"

#include "log.h"
#include "compose.hpp"
#include "Remotery.h"

#include <boost/algorithm/string.hpp>
#include <boost/asio/steady_timer.hpp>

FileSystemObserver::FileSystemObserver(boost::asio::io_context& ioService)
    : m_ioService(ioService)
{
    try {
        mDirectoryMonitor = std::make_unique<boost::asio::dir_monitor>(ioService);
        observe();
    } catch (const boost::exception& e) {
        log(WARNING, "Could not initialize file system observer; probably due to running out of file descriptors.");
    }
}

FileSystemObserver::~FileSystemObserver()
{
    assert(mCallBacks.empty());
}

void FileSystemObserver::stop()
{
    mDirectoryMonitor.reset();
}


void FileSystemObserver::observe()
{
    if (mDirectoryMonitor) {
        mDirectoryMonitor->async_monitor([this](const boost::system::error_code& ec, const boost::asio::dir_monitor_event& ev) {
            if (!ec && ev.type != boost::asio::dir_monitor_event::null) {
                //Don't process directly, since there might be many events coming directly after each other. Instead
                //add to the map of changed paths and schedule a processing in 2 milliseconds
                m_changedPaths.emplace(ev.path, std::make_pair(std::chrono::steady_clock::now(), ev));
                processChangedPaths();
                this->observe();
            }
        });
    }
}

void FileSystemObserver::add_directory(const boost::filesystem::path& dirname, std::function<void(const FileSystemObserver::FileSystemEvent&)> callback)
{
    if (mDirectoryMonitor) {
        try {
            mDirectoryMonitor->add_directory(dirname.string());
        } catch (...) {
            log(WARNING, String::compose("Could not observe directory %1", dirname.string()));
        }
        mCallBacks.emplace(dirname, std::move(callback));
    }
}

void FileSystemObserver::remove_directory(const boost::filesystem::path& dirname)
{
    mCallBacks.erase(dirname);
    if (mDirectoryMonitor) {
        try {
            mDirectoryMonitor->remove_directory(dirname.string());
        } catch (...) {
            //Just swallow exceptions when removing watches; this is often because the directory has been removed. Doesn't change anything.
        }
    }
}

void FileSystemObserver::processChangedPaths()
{
    rmt_ScopedCPUSample(processChangedPaths, 0)
    if (!m_changedPaths.empty()) {
        auto firstI = m_changedPaths.begin();
        //check if enough time has passed for the first entry
        if (firstI->second.first <= std::chrono::steady_clock::now() - std::chrono::milliseconds(2)) {
            for (const auto& I : mCallBacks) {
                auto& ev = firstI->second.second;
                if (boost::starts_with(ev.path.string(), I.first.string())) {
                    std::string relative = ev.path.string().substr(I.first.string().length() + 1);
                    FileSystemEvent event{
                        ev,
                        relative
                    };
                    I.second(event);

                    break;
                }
            }

            m_changedPaths.erase(firstI);
        }

        if (!m_changedPaths.empty()) {
            auto& time_point = m_changedPaths.begin()->second.first;
            auto timer = std::make_shared<boost::asio::steady_timer>(m_ioService);
            timer->expires_at(time_point + std::chrono::milliseconds(2));
            timer->async_wait([&, timer](const boost::system::error_code& ec) {
                if (!ec) {
                    processChangedPaths();
                }
            });
        }
    }

}
