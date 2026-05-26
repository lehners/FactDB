// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/QueryHandle.hpp"
#include <cassert>
#include <iostream>
#include <dlfcn.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
RawHandle::RawHandle(const std::string& pathP, bool global) noexcept : path(std::move(pathP)) {
   std::cout << "try load " << path << std::endl;
#ifndef __APPLE__
   if (auto probe = dlopen(path.c_str(), RTLD_NOLOAD)) {
      dlclose(probe);
      assert(false);
   }
#endif

   auto flags = global ? (RTLD_NOW | RTLD_GLOBAL) : RTLD_NOW;
   handle = dlopen(path.c_str(), flags);
   if (!handle) {
      std::cerr << "error: " << dlerror() << std::endl;
      assert(false);
   }
   std::cout << "loaded   " << path << std::endl;
}
// ---------------------------------------------------------------------------------------------------
RawHandle::~RawHandle() {
   std::cout << "unload   " << path << std::endl;
   if (handle && dlclose(handle)) {
      std::cerr << "error: " << dlerror() << std::endl;
      assert(false);
   }

#ifndef __APPLE__
   if (auto probe = dlopen(path.c_str(), RTLD_NOLOAD)) {
      dlclose(probe);
      assert(false && "library was not unloaded for some reasons");
   }
#endif
}
// ---------------------------------------------------------------------------------------------------
RawHandle::RawHandle(RawHandle&& other) noexcept : handle(other.handle), path(std::move(other.path)) {
   other.handle = nullptr;
}
// ---------------------------------------------------------------------------------------------------
RawHandle& RawHandle::operator=(RawHandle&& other) noexcept {
   if (this != &other) {
      handle = other.handle;
      path = std::move(other.path);
      other.handle = nullptr;
   }
   return *this;
}
// ---------------------------------------------------------------------------------------------------
void* RawHandle::loadFunctionRaw(const std::string& functionName) const {
   auto function_pointer = dlsym(handle, functionName.c_str());
   if (!function_pointer) {
      std::cerr << "error: " << dlerror() << std::endl;
      assert(false);
   }
   return function_pointer;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------