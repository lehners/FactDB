#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Types.hpp"
#include "factDB/util/FileReader.hpp"
#include <cstdint>
#include <fstream>
#include <istream>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct RuntimeValue;
class IU;
// ---------------------------------------------------------------------------------------------------
struct BaseTable {
   bool loaded = false;

   virtual ~BaseTable() = default;
   virtual void loadRelation(const std::string& in, char separator = ',') = 0;
   virtual void unloadRelation() = 0;
   [[nodiscard]] virtual size_t size() const = 0;
   [[nodiscard]] virtual std::string_view name() const = 0;
   virtual void genTblFiles(const std::string& tblPath, char separator) const = 0;

   [[nodiscard]] virtual RuntimeValue get(size_t idx, const IU& val) const = 0;

   template <typename T>
      requires isFactDBType<T>::value
   static void updateElem(T& val, std::istream& in, char separator, bool end) { // NOLINT
      std::string buffer;
      std::getline(in, buffer, end ? '\n' : separator);
      assert(!in.eof());
      val = val.castString(buffer.c_str(), buffer.size());
   }

   template <typename Tbl, typename Container>
   static void genTblFileImpl(const std::string& csvPath, char separator) {
      Tbl relation;
      relation.loadRelation(csvPath, separator);

      auto tblFile = csvPath + ".tbl";
      FILE* pfile = fopen(tblFile.c_str(), "wb");
      for (auto& container : relation.data)
         fwrite(&container, 1, sizeof(Container), pfile);
      fclose(pfile);
   }

   template </*typename Tbl,*/ typename Container>
   static void readTblFile([[maybe_unused]] std::string tblFile, [[maybe_unused]] std::vector<Container>& data_vector) {
      FILE* pfile = fopen(tblFile.c_str(), "rb");
      long fileSize = 0;
      {
         // Seek to end to get size
         fseek(pfile, 0, SEEK_END);
         fileSize = ftell(pfile);
         rewind(pfile); // reset to beginning

         if (fileSize < 0) {
            fclose(pfile);
            throw std::runtime_error("Failed to get file size");
         }
      }

      size_t vecSize = fileSize / sizeof(Container);
      data_vector.resize(vecSize);
      size_t n = fread(data_vector.data(), sizeof(Container), vecSize, pfile);
      if (n != vecSize) {
         if (feof(pfile))
            printf("Error reading test.bin: unexpected end of file\n");
         else if (auto fer = ferror(pfile); fer)
            perror("Error reading test.bin");
         throw std::runtime_error("failed to rad all tuples");
      }
      fclose(pfile);
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------