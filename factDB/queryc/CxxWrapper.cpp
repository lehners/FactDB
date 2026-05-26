#include "factDB/queryc/CxxWrapper.hpp"
#include "factDB/config.h"
#include "factDB/infra/Config.hpp"
#include "factDB/queryc/CxxCache.hpp"
#include "factDB/queryc/QueryHandle.hpp"
#include <iostream>
#include <fmt/format.h>
#include <oneapi/tbb/parallel_for_each.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
std::string getDebugFlag() {
   if constexpr (CURRENT_BUILD_MODE == "RelWithDebInfo"sv) { // NOLINT
      return "-g -DNDEBUG";
   } else if constexpr (CURRENT_BUILD_MODE == "Debug"sv) { // NOLINT
      return "-g";
   } else if constexpr (CURRENT_BUILD_MODE == "Release"sv) {
      return "-DNDEBUG";
   } else {
      std::cerr << "unknown build mode \"" << CURRENT_BUILD_MODE << "\", defaulting to release!" << std::endl;
      return "-DNDEBUG";
   }
}
// ---------------------------------------------------------------------------------------------------
std::string getLgcovIfRequired() {
#if ENABLE_COVERAGE
   return "-lgcov";
#else
   return "";
#endif
}
// ---------------------------------------------------------------------------------------------------
std::string linkFmt() {
   // -L path_to_lib -l lib
   if constexpr (debugMode) {
      return fmt::format("-L{}/_deps/fmt-build/ -lfmtd", CURRENT_BINARY_DIR); // debug build
   } else {
      return fmt::format("-L{}/_deps/fmt-build/ -lfmt", CURRENT_BINARY_DIR); // release build
   }
}
// ---------------------------------------------------------------------------------------------------
size_t optimizationLevel() {
   if constexpr (debugMode) {
      return 0;
   } else {
      return 3;
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
std::string CxxCompiler::getRequiredLibs() {
   return fmt::format("-L {} -L /opt/homebrew/lib -lfactDB -ltbb -ltbbmalloc {} {} {} -L /opt/homebrew/Cellar/libpq/18.3/lib/ -lpq",
                      CURRENT_BINARY_DIR,
                      linkFmt(),
                      getLgcovIfRequired(),
                      (requiresGtest ? "-lgtest" : ""));
}
// ---------------------------------------------------------------------------------------------------
std::string CxxCompiler::getRequiredIncludes() {
   return fmt::format("-I {1} -I {0}/_deps/fmt-src/include -I /opt/homebrew/include",
                      CURRENT_BINARY_DIR, CURRENT_SRC_DIR);
}
// ---------------------------------------------------------------------------------------------------
std::string CxxCompiler::compile(std::string_view source, bool useCache) { // NOLINT
   auto& cxxCache = CxxCache::getCache();
   auto cached_so = useCache ? cxxCache[std::string(source)] : nullptr;
   if (useCache && cached_so != nullptr) {
      if constexpr (debugMode) {
         std::cout << "compile file: " << source << std::endl
                   << "cached file : " << *cached_so << std::endl;
      }
      return *cached_so;
   } else {
      std::string dst = fmt::format("{}.so", source);
      std::string include = CURRENT_BINARY_DIR + std::string("/../.");
      std::string cmd = fmt::format(CMAKE_CXX_COMPILER " {0} -fPIC -shared -fno-strict-aliasing -std=c++2b -O{1} -o {2} {3} {4} {5}",
                                    getDebugFlag(), optimizationLevel(), dst, getRequiredIncludes(), source, getRequiredLibs());
      if constexpr (debugMode) {
         std::cout << cmd << std::endl;
         std::cout << "compile file: " << source << std::endl
                   << "cached file : no file found!" << std::endl;
      }
      if (std::system(cmd.c_str()))
         exit(1);
      if (useCache)
         cxxCache.add(std::string(source), dst);
      return dst;
   }
}
// ---------------------------------------------------------------------------------------------------
RawHandle CxxCompiler::compile(std::string_view source, bool useCache, bool loadGlobal, factDB::queryc::CxxCompiler::rawHandleTag) {
   return {compile(source, useCache), loadGlobal};
}
// ---------------------------------------------------------------------------------------------------
RawHandle CxxCompiler::compile(const std::unordered_set<std::string>& sources, const std::string& dst, bool useCache, bool loadGlobal, rawHandleTag) {
   assert(dst.ends_with(".so"));
   auto& cxxCache = CxxCache::getCache();
   std::string summarySrc = dst.substr(0, dst.size() - 3);
   auto cached_so = useCache ? cxxCache[std::string(summarySrc)] : nullptr;
   if (useCache && cached_so != nullptr) {
      if constexpr (debugMode) {
         std::cout << "compile file: " << summarySrc << std::endl
                   << "cached file : " << *cached_so << std::endl;
      }
      return *cached_so;
   } else {
      std::string include = CURRENT_BINARY_DIR + std::string("/../.");
      std::string sourcesStr = "";

      if constexpr (false) {
         tbb::parallel_for_each(sources.begin(), sources.end(), [&](auto cur_src) {
            std::string cmd = fmt::format(CMAKE_CXX_COMPILER " {0} -fPIC -fno-strict-aliasing -std=c++2a -O{1} -c -o {2}.o {3} {2}",
                                          getDebugFlag(), optimizationLevel(), cur_src, getRequiredIncludes());
            if constexpr (debugMode)
               std::cout << cmd << std::endl;
            if (std::system(cmd.c_str()))
               exit(1);
         });

         for (auto cur_src : sources)
            sourcesStr += fmt::format(" {}.o", cur_src);
         std::string linkCmd = fmt::format(CMAKE_CXX_COMPILER " -fPIC -fno-strict-aliasing -shared -o {0} {1} {2}",
                                           dst, sourcesStr, getRequiredLibs());
         if (std::system(linkCmd.c_str()))
            exit(1);
      } else {
         for (auto cur_src : sources)
            sourcesStr += std::string(cur_src) + " ";

         std::string cmd = fmt::format(CMAKE_CXX_COMPILER " {0} -fPIC -fno-strict-aliasing -shared -std=c++2a -O{1} -o {2} {3} {4} {5}",
                                       getDebugFlag(), optimizationLevel(), dst, getRequiredIncludes(), sourcesStr, getRequiredLibs());
         if constexpr (debugMode) {
            std::cout << cmd << std::endl;
            std::cout << "compile files: " << sourcesStr << std::endl
                      << "cached file : no file found (multiple sources)!" << std::endl;
         }
         if (std::system(cmd.c_str()))
            exit(1);
         if (useCache)
            cxxCache.add(summarySrc, dst);
      }
      return {dst, loadGlobal};
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
