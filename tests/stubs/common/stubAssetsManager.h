// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubAssetsManager_custom.h file.

#ifndef STUB_COMMON_ASSETSMANAGER_H
#define STUB_COMMON_ASSETSMANAGER_H

#include "common/AssetsManager.h"
#include "stubAssetsManager_custom.h"

#ifndef STUB_AssetsManager_AssetsManager
//#define STUB_AssetsManager_AssetsManager
   AssetsManager::AssetsManager(FileSystemObserver& file_system_observer)
    : Singleton(file_system_observer)
  {
    
  }
#endif //STUB_AssetsManager_AssetsManager

#ifndef STUB_AssetsManager_AssetsManager_DTOR
//#define STUB_AssetsManager_AssetsManager_DTOR
   AssetsManager::~AssetsManager()
  {
    
  }
#endif //STUB_AssetsManager_AssetsManager_DTOR

#ifndef STUB_AssetsManager_init
//#define STUB_AssetsManager_init
  void AssetsManager::init()
  {
    
  }
#endif //STUB_AssetsManager_init

#ifndef STUB_AssetsManager_observeFile
//#define STUB_AssetsManager_observeFile
  void AssetsManager::observeFile(boost::filesystem::path path, const std::function<void(const boost::filesystem::path& path)>& callback)
  {
    
  }
#endif //STUB_AssetsManager_observeFile

#ifndef STUB_AssetsManager_observeDirectory
//#define STUB_AssetsManager_observeDirectory
  void AssetsManager::observeDirectory(boost::filesystem::path path, const std::function<void(const boost::filesystem::path& path)>& callback)
  {
    
  }
#endif //STUB_AssetsManager_observeDirectory


#endif