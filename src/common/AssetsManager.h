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

#ifndef CYPHESIS_ASSETSMANAGER_H
#define CYPHESIS_ASSETSMANAGER_H


#include "Singleton.h"
#include <boost/filesystem/path.hpp>
#include <list>
#include <map>
#include <filesystem>

class FileSystemObserver;

class AssetsManager : public Singleton<AssetsManager>
{
    public:

        explicit AssetsManager(FileSystemObserver& file_system_observer);
        ~AssetsManager() override;

        void init();

        void observeFile(boost::filesystem::path path, const std::function<void(const boost::filesystem::path& path)>& callback);
        void observeDirectory(boost::filesystem::path path, const std::function<void(const boost::filesystem::path& path)>& callback);

        std::filesystem::path getAssetsPath() const { return mAssetsPath;}
    private:

        FileSystemObserver& m_file_system_observer;

        std::filesystem::path mAssetsPath;

        std::map<boost::filesystem::path, std::list<std::function<void(const boost::filesystem::path& path)>>> m_callbacks;
        std::map<boost::filesystem::path, std::list<std::function<void(const boost::filesystem::path& path)>>> m_directoryCallbacks;

};


#endif //CYPHESIS_ASSETSMANAGER_H
