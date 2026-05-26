#ifndef H_FACTDB_QUERYC_CXXCACHE_HPP
#define H_FACTDB_QUERYC_CXXCACHE_HPP
// ---------------------------------------------------------------------------------------------------
#include <filesystem>
#include <string>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
struct string_hash {
   using hash_type = std::hash<std::string_view>;
   using is_transparent = void;

   std::size_t operator()(const char* str) const { return hash_type{}(str); }
   std::size_t operator()(std::string_view str) const { return hash_type{}(str); }
   std::size_t operator()(std::string const& str) const { return hash_type{}(str); }
};
// ---------------------------------------------------------------------------------------------------
class CxxCache {
   public:
   using HashTableType = std::unordered_map<std::string, std::tuple<std::string, std::filesystem::file_time_type>, string_hash, std::equal_to<>>;

   private:
   std::unordered_map<std::string, std::string, string_hash, std::equal_to<>> cxx2file;
   std::unordered_map<std::string, std::tuple<std::string, std::filesystem::file_time_type>, string_hash, std::equal_to<>> file2so;

   std::tuple<size_t, size_t> loadFolder(const std::string_view& path);

   public:
   CxxCache() { load(); };

   void load();
   bool add(const std::filesystem::path& srcFile);
   bool add(const std::string& srcFile, const std::string& compiledFile);

   const std::string* operator[](const std::string& filename);
   bool empty() { return cxx2file.empty() && file2so.empty(); }

   static CxxCache& getCache();
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_QUERYC_CXXCACHE_HPP