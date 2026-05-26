#ifndef H_FACTDB_CXXWRAPPER_HPP
#define H_FACTDB_CXXWRAPPER_HPP
// ---------------------------------------------------------------------------------------------------
#include <filesystem>
#include <string_view>
#include <unordered_set>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
class RawHandle;
// ---------------------------------------------------------------------------------------------------
class CxxCompiler {
   public:
   enum rawHandleTag { RawHandleTag };

   private:
   uint8_t optimize = 3;
   bool requiresGtest = false;

   public:
   CxxCompiler() = default;
   std::string compile(std::string_view source, bool useCache = true);
   RawHandle compile(std::string_view source, bool useCache, bool loadGlobal, rawHandleTag);
   RawHandle compile(const std::unordered_set<std::string>& sources, const std::string& dst, bool useCache, bool loadGlobal, rawHandleTag);

   void setOptimizerLevel(uint8_t level) { optimize = level; };
   void useGtest(bool use = true) { requiresGtest = use; };

   private:
   std::string getRequiredLibs();
   std::string getRequiredIncludes();
   std::string getSourceFiles(std::string_view source);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_CXXWRAPPER_HPP
