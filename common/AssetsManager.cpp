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

#include "AssetsManager.h"
#include "FileSystemObserver.h"
#include "globals.h"

template<> AssetsManager* Singleton<AssetsManager>::ms_Singleton = nullptr;

AssetsManager::AssetsManager(FileSystemObserver& file_system_observer)
    : m_file_system_observer(file_system_observer)
{

}

void AssetsManager::init()
{

    auto observerCallback = [&](const FileSystemObserver::FileSystemEvent& event) {
        auto I = m_callbacks.find(event.ev.path);
        if (I != m_callbacks.end()) {
            for (auto& callback : I->second) {
                callback(event.ev.path);
            }
        }
    };


    //TODO: implement for all asset paths
    m_file_system_observer.add_directory(boost::filesystem::path(share_directory) / "cyphesis" / "scripts", observerCallback);
    m_file_system_observer.add_directory(boost::filesystem::path(share_directory) / "cyphesis" / "rulesets", observerCallback);
    m_file_system_observer.add_directory(boost::filesystem::path(assets_directory), observerCallback);

    m_file_system_observer.add_directory(boost::filesystem::path(etc_directory) / "cyphesis", observerCallback);


}

void AssetsManager::observeFile(boost::filesystem::path path, const std::function<void(const boost::filesystem::path& path)>& callback)
{

    m_callbacks[path].push_back(callback);

}
