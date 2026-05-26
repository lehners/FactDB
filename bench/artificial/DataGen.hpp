#pragma once
// ---------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Database;
// ---------------------------------------------------------------------------------------------------
struct DataGen {
   private:
   using DataType = std::vector<std::tuple<size_t, size_t>>;
   using Tables = std::tuple<DataType, DataType, DataType>;

   std::string out_path;

   DataType table1;
   DataType table2;
   DataType table3;

   size_t size1;
   size_t size2;
   size_t size3;

   DataGen(const std::string& out_path_) : out_path(out_path_) {}

   public:
   void write(char separator = '|') const;
   std::string getTableName(size_t table) const;
   std::string getTableOutputName(size_t table, bool requiresPrefix = false, bool csvSuffix = false) const;
   const DataType& getTable(size_t table) const;
   size_t getTableSize(size_t table) const;
   void shuffle();
   void reserve(size_t s1, size_t s2, size_t s3);
   const std::string& getOutPath() const { return out_path; }

   static DataGen genDataNoDuplicates(size_t s1, size_t s2, size_t s3);
   static DataGen genUniformDistribution(size_t s1, size_t s2, size_t s3, size_t r1, size_t r2, size_t r_3);
   static DataGen genZipfDistribution(size_t s1, size_t s2, size_t s3, size_t n, double alpha = 1.0);

   std::string getLoadPath() const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
