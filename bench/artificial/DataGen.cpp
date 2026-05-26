// ---------------------------------------------------------------------------------------------------
#include "bench/artificial/DataGen.hpp"
#include "factDB/infra/Setting.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_set>
#include <factDB/config.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> generateTablesSetting("bench.artificial.generateData", true);
// ---------------------------------------------------------------------------------------------------
std::string DataGen::getTableName(size_t table) const {
   return fmt::format("v{}x{}", table, getTable(table).size());
}
// ---------------------------------------------------------------------------------------------------
std::string DataGen::getTableOutputName(size_t table, bool requiresPrefix, bool csvSuffix) const {
   auto full_out_path = requiresPrefix ? CURRENT_SRC_DIR + out_path + "/" : "";
   assert(table > 0 && table < 4);
   return fmt::format("{}{}.{}", full_out_path, getTableName(table), csvSuffix ? "csv" : "tbl");
}
// ---------------------------------------------------------------------------------------------------
const DataGen::DataType& DataGen::getTable(size_t table) const {
   switch (table) {
      case 1: return table1;
      case 2: return table2;
      case 3: return table3;
      default: __builtin_unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
size_t DataGen::getTableSize(size_t table) const {
   switch (table) {
      case 1: return size1;
      case 2: return size2;
      case 3: return size3;
      default: __builtin_unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void DataGen::write(char separator) const {
   // write the data in different formats to files, also with the corresponding loading scripts
   if (!generateTablesSetting.get()) {
      return; // do nothing
   }

   if (!std::filesystem::exists(CURRENT_SRC_DIR + out_path))
      std::filesystem::create_directories(CURRENT_SRC_DIR + out_path);

   std::cout << "data output path: " << CURRENT_SRC_DIR << out_path << std::endl;
   auto writeData = [separator](const DataType& vec, const std::string& outputPath, bool swapFirstColumns) {
      std::ofstream f1(outputPath);
      for (size_t idx = 0; idx != vec.size(); ++idx) {
         auto& [val1, val2] = vec[idx];
         auto v1 = swapFirstColumns ? val2 : val1;
         auto v2 = swapFirstColumns ? val1 : val2;
         f1 << v1 << separator << v2 << separator << idx << std::endl;
      }
   };

   writeData(table1, getTableOutputName(1, true), true);
   writeData(table2, getTableOutputName(2, true), false);
   writeData(table3, getTableOutputName(3, true), false);

   {
      std::unordered_set<size_t> keys;
      auto insertKeys = [&keys](const DataType& vec) {
         for (auto& v : vec) {
            keys.insert(std::get<0>(v));
            keys.insert(std::get<1>(v));
         }
      };

      insertKeys(table1);
      insertKeys(table2);
      insertKeys(table3);

      auto full_out_path = CURRENT_SRC_DIR + out_path + "/artificial_keys.csv";
      std::cout << full_out_path << std::endl;
      std::ofstream artificial_keys_file(full_out_path);
      for (size_t k : keys)
         artificial_keys_file << k << std::endl;
   }

   {
      auto full_out_path = CURRENT_SRC_DIR + out_path + "/load.cypher";
      std::ofstream loadCyper(full_out_path);
      loadCyper << fmt::format("copy artificial_keys from \".{}/artificial_keys.csv\" (header=false, delim=\"|\");", out_path) << std::endl
                << fmt::format("copy artificial1 from \".{}/{}\" (header=false, delim=\"|\");", out_path, getTableOutputName(1, false, true)) << std::endl
                << fmt::format("copy artificial2 from \".{}/{}\" (header=false, delim=\"|\");", out_path, getTableOutputName(2, false, true)) << std::endl
                << fmt::format("copy artificial3 from \".{}/{}\" (header=false, delim=\"|\");", out_path, getTableOutputName(3, false, true)) << std::endl;
      if (auto symPath = getTableOutputName(1, true, true); !std::filesystem::exists(symPath)) std::filesystem::create_symlink(getTableOutputName(1, true, false), symPath);
      if (auto symPath = getTableOutputName(2, true, true); !std::filesystem::exists(symPath)) std::filesystem::create_symlink(getTableOutputName(2, true, false), symPath);
      if (auto symPath = getTableOutputName(3, true, true); !std::filesystem::exists(symPath)) std::filesystem::create_symlink(getTableOutputName(3, true, false), symPath);
   }
   {
      std::ofstream loadSQL(getLoadPath());
      loadSQL << fmt::format("copy artificial1 from '.{}/{}\' DELIMITER '|';", out_path, getTableOutputName(1, false)) << std::endl
              << fmt::format("copy artificial2 from '.{}/{}\' DELIMITER '|';", out_path, getTableOutputName(2, false)) << std::endl
              << fmt::format("copy artificial3 from '.{}/{}\' DELIMITER '|';", out_path, getTableOutputName(3, false)) << std::endl;
   }
   { // FDB stuff
      auto fdb_schema_file = CURRENT_SRC_DIR + out_path + "/schema.conf";
      std::ofstream fdb_schema_stream(fdb_schema_file);
      fdb_schema_stream << getTableName(1) << ":b,a,x" << std::endl
                        << getTableName(2) << ":b,c,y" << std::endl
                        << getTableName(3) << ":c,d,z" << std::endl;
      fdb_schema_stream.close();
   }
   { // FDB stuff
      auto fdb_dtree_file = CURRENT_SRC_DIR + out_path + "/dtree.txt";
      std::ofstream fdb_dtree_stream(fdb_dtree_file);
      fdb_dtree_stream << "#" << std::endl
                       << "7 3" << std::endl
                       << "#" << std::endl
                       << "0 b int -1 {} 0" << std::endl
                       << "1 a int 0 {0} 0" << std::endl
                       << "2 c int 1 {0} 0" << std::endl
                       << "3 d int 2 {0,2} 0" << std::endl
                       << "4 x int 1 {0,1} 0" << std::endl
                       << "5 y int 3 {0,2,3} 0" << std::endl
                       << "6 z int 3 {0,2,3} 0" << std::endl
                       << "#" << std::endl
                       << getTableName(1) << " 4 a,b,x" << std::endl
                       << getTableName(2) << " 5 b,c,y" << std::endl
                       << getTableName(3) << " 6 c,d,z" << std::endl;
   }
   { // LMFAO stuff part 1:
      auto fdb_dtree_file = CURRENT_SRC_DIR + out_path + "/features.conf";
      std::ofstream fdb_dtree_stream(fdb_dtree_file);
      fdb_dtree_stream << "#####" << std::endl
                       << "1, 0, 0" << std::endl
                       << "b:0:" << getTableName(1) << std::endl;
   }
   { // LMFAO stuff part 2:
      auto fdb_dtree_file = CURRENT_SRC_DIR + out_path + "/treedecomposition.conf";
      std::ofstream fdb_dtree_stream(fdb_dtree_file);
      fdb_dtree_stream << "#####" << std::endl
                       << "7 3 2" << std::endl
                       << "#### 2) Attributes in Join Tree" << std::endl
                       << "0 a int" << std::endl
                       << "1 b int" << std::endl
                       << "2 c int" << std::endl
                       << "3 d double" << std::endl
                       << "4 x int" << std::endl
                       << "5 y double" << std::endl
                       << "6 z int" << std::endl
                       << "#### 3) Relations in Join Tree" << std::endl
                       << "0 " << getTableName(1) << ":a,b,x" << std::endl
                       << "1 " << getTableName(2) << ":a,c,y" << std::endl
                       << "2 " << getTableName(3) << ":c,d,z" << std::endl
                       << "#### 4) Edges in Join Tree" << std::endl
                       << getTableName(1) << "-" << getTableName(2) << std::endl
                       << getTableName(2) << "-" << getTableName(3) << std::endl
                       << "#### 5) Threads per Relation" << std::endl
                       << getTableName(1) << " 1" << std::endl
                       << getTableName(2) << " 1" << std::endl
                       << getTableName(3) << " 1" << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
std::string DataGen::getLoadPath() const {
   return CURRENT_SRC_DIR + out_path + "/load.sql";
}
// ---------------------------------------------------------------------------------------------------
void DataGen::shuffle() {
   // shuffle all tables
   auto rng = std::default_random_engine{};
   std::shuffle(table1.begin(), table1.end(), rng);
   std::shuffle(table2.begin(), table2.end(), rng);
   std::shuffle(table3.begin(), table3.end(), rng);
}
// ---------------------------------------------------------------------------------------------------
void DataGen::reserve(size_t s1, size_t s2, size_t s3) {
   table1.reserve(s1);
   table2.reserve(s2);
   table3.reserve(s3);

   size1 = s1;
   size2 = s2;
   size3 = s3;
}
// ---------------------------------------------------------------------------------------------------
DataGen DataGen::genDataNoDuplicates(size_t s1, size_t s2, size_t s3) {
   // generate data: dense int values in [0,s_x)
   DataGen gen(fmt::format("/bench/data/micro/simple{}x{}x{}", s1, s2, s3));
   gen.reserve(s1, s2, s3);
   if (!generateTablesSetting.get()) {
      if (!std::filesystem::exists(gen.getLoadPath()))
         std::cout << "Data does not exist in \"" << gen.out_path << "\". Make sure to provide the input data." << std::endl;
      return gen; // do not generate data, just return empty DataGen
   }

   for (size_t i = 0; i < s1; i++)
      gen.table1.emplace_back(i, i);
   for (size_t i = 0; i < s2; i++)
      gen.table2.emplace_back(i, i);
   for (size_t i = 0; i < s3; i++)
      gen.table3.emplace_back(i, i);

   return gen;
}
// ---------------------------------------------------------------------------------------------------
DataGen DataGen::genUniformDistribution(size_t s1, size_t s2, size_t s3, size_t r1, size_t r2, size_t r3) {
   // generate data: deterministically sampled from uniform distribution in [1, r_x] (fixed seed 123)
   DataGen dataGen(fmt::format("/bench/data/micro/uniform_{}x{}x{}_{}x{}x{}", s1, s2, s3, r1, r2, r3));
   dataGen.reserve(s1, s2, s3);
   if (!generateTablesSetting.get()) {
      // check that input files actually exist
      if (!std::filesystem::exists(dataGen.getLoadPath()))
         std::cout << "Data does not exist in \"" << dataGen.out_path << "\". Make sure to provide the input data." << std::endl;
      return dataGen; // do not generate data, just return empty DataGen
   }

   std::random_device rd; // a seed source for the random number engine
   std::mt19937 generator(123);
   std::uniform_int_distribution<> d1(1, r1);
   std::uniform_int_distribution<> d2(1, r2);
   std::uniform_int_distribution<> d3(1, r3);

   for (size_t i = 0; i < s1; i++)
      dataGen.table1.emplace_back(d1(generator), d1(generator));
   for (size_t i = 0; i < s2; i++)
      dataGen.table2.emplace_back(d2(generator), d2(generator));
   for (size_t i = 0; i < s3; i++)
      dataGen.table3.emplace_back(d3(generator), d3(generator));

   return dataGen;
}
// ---------------------------------------------------------------------------------------------------
DataGen DataGen::genZipfDistribution(size_t s1, size_t s2, size_t s3, size_t n, double alpha) {
   // Generate data with Zipfian distribution over [1, n] with skew parameter alpha (fixed seed 123).
   // The CDF is precomputed by normalizing the harmonic series weights 1/i^alpha for i in [1, n].
   // Values are sampled via inverse transform sampling: a uniform random value is mapped to a
   // Zipfian rank using binary search on the CDF.

   DataGen dataGen(fmt::format("/bench/data/micro/zipf_{}x{}x{}_{}_{}", s1, s2, s3, n, alpha));
   dataGen.reserve(s1, s2, s3);

   if (!generateTablesSetting.get()) {
      if (!std::filesystem::exists(dataGen.getLoadPath()))
         std::cout << "Data does not exist in \"" << dataGen.out_path << "\". Make sure to provide the input data." << std::endl;
      return dataGen; // do not generate data, just return empty DataGen
   }

   // FDB: A Query Engine for Factorised Relational Databases => zipf with range [1, 100] up to 100'000 elements
   std::random_device rd;
   std::mt19937 generator(123);
   std::uniform_real_distribution<> dis(0.0, 1.0);

   std::vector<double> cdf; // cummulative distribution function
   { // precompute cdf for n values with skew alpha
      double sum = 0;
      cdf.resize(n + 1);
      cdf[0] = 0;
      for (size_t i = 1; i <= n; i++) {
         sum += 1.0 / std::pow(i, alpha);
         cdf[i] = sum;
      }

      for (size_t i = 1; i <= n; i++) { // Normalize CDF
         cdf[i] /= sum;
      }
   }

   auto generateNumber = [&]() {
      double nextVal = dis(generator);
      auto bound = std::lower_bound(cdf.begin(), cdf.end(), nextVal);
      return bound - cdf.begin();
   };

   for (size_t i = 0; i < s1; i++)
      dataGen.table1.emplace_back(generateNumber(), generateNumber());
   for (size_t i = 0; i < s2; i++)
      dataGen.table2.emplace_back(generateNumber(), generateNumber());
   for (size_t i = 0; i < s3; i++)
      dataGen.table3.emplace_back(generateNumber(), generateNumber());

   return dataGen;
}
// ---------------------------------------------------------------------------------------------------
