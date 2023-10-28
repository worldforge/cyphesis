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

#include <boost/algorithm/string.hpp>
#include "AssetsManager.h"
#include "FileSystemObserver.h"
#include "globals.h"
#include "log.h"
#include "compose.hpp"

AssetsManager::AssetsManager(FileSystemObserver& file_system_observer)
        : m_file_system_observer(file_system_observer) {
    std::filesystem::path assetsDirectory(assets_directory);
    if (std::filesystem::exists(assetsDirectory)) {
        mAssetsPath = assetsDirectory;
    } else {
        mAssetsPath = std::filesystem::path(CYPHESIS_RAW_ASSETS_DIRECTORY);
        if (std::filesystem::exists(mAssetsPath)) {
            log(INFO, String::compose("Could not find any assets directory in '%1' but found raw assets in '%2'.", assets_directory, CYPHESIS_RAW_ASSETS_DIRECTORY));
        } else {
            log(ERROR,
                String::compose("Could not find neither assets directory in '%1' or found raw assets in '%2'. Will continue but the server will probably not function correctly.", assets_directory,
                                CYPHESIS_RAW_ASSETS_DIRECTORY));
        }
    }

}

AssetsManager::~AssetsManager() {
    m_file_system_observer.remove_directory(boost::filesystem::path(share_directory) / "cyphesis" / "scripts");
    m_file_system_observer.remove_directory(boost::filesystem::path(share_directory) / "cyphesis" / "rulesets");
    m_file_system_observer.remove_directory(boost::filesystem::path(mAssetsPath));
    m_file_system_observer.remove_directory(boost::filesystem::path(etc_directory) / "cyphesis");

}

void AssetsManager::init() {

    auto observerCallback = [&](const FileSystemObserver::FileSystemEvent& event) {
        auto I = m_callbacks.find(event.ev.path);
        if (I != m_callbacks.end()) {
            for (auto& callback: I->second) {
                callback(event.ev.path);
            }
        }

        for (auto& entry: m_directoryCallbacks) {

            if (boost::starts_with(event.ev.path.string(), entry.first.string())) {
                if (!boost::filesystem::is_directory(event.ev.path)) {
                    for (auto& callback: entry.second) {
                        callback(event.ev.path);
                    }
                } else {
                    //Handle new files and directories being added. It's trickier with stuff being removed though.
                    if (event.ev.type == boost::asio::dir_monitor_event::added) {
                        boost::filesystem::recursive_directory_iterator dir(event.ev.path), end{};
                        while (dir != end) {
                            if (!boost::filesystem::is_directory(dir->status())) {
                                for (auto& callback: entry.second) {
                                    callback(dir->path());
                                }
                            }
                            ++dir;
                        }
                    }
                }
            }
        }
    };


    //TODO: implement for all asset paths
    m_file_system_observer.add_directory(boost::filesystem::path(share_directory) / "cyphesis" / "scripts", observerCallback);
    m_file_system_observer.add_directory(boost::filesystem::path(share_directory) / "cyphesis" / "rulesets", observerCallback);
    m_file_system_observer.add_directory(boost::filesystem::path(mAssetsPath), observerCallback);

    m_file_system_observer.add_directory(boost::filesystem::path(etc_directory) / "cyphesis", observerCallback);


}

void AssetsManager::observeFile(boost::filesystem::path path, const std::function<void(const boost::filesystem::path& path)>& callback) {
    m_callbacks[std::move(path)].push_back(callback);
}

void AssetsManager::observeDirectory(boost::filesystem::path path, const std::function<void(const boost::filesystem::path& path)>& callback) {
    m_directoryCallbacks[std::move(path)].push_back(callback);
}
