#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
struct EpinionsBase : public BaseTable {
   enum Columns { s,
                  d };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) s(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) d(this auto&& self) { return std::get<1>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~EpinionsBase() override = default;
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;

   protected:
   static schemac::Table genSchema(uint32_t id, std::string name);
};
// ---------------------------------------------------------------------------------------------------
struct epinions75888 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75888"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75889 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75889"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75890 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75890"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75891 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75891"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75892 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75892"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75893 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75893"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75894 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75894"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75895 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75895"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75896 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75896"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75897 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75897"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75898 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75898"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75899 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75899"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75900 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75900"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75901 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75901"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75902 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75902"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75903 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75903"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75904 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75904"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75905 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75905"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75906 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75906"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75907 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75907"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75908 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75908"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75909 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75909"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75910 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75910"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75911 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75911"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75912 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75912"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75913 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75913"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75914 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75914"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75915 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75915"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75916 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75916"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75917 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75917"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75918 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75918"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75919 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75919"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75920 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75920"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75921 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75921"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75922 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75922"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75923 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75923"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75924 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75924"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75925 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75925"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75926 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75926"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75927 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75927"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75928 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75928"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75929 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75929"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75930 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75930"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75931 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75931"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75932 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75932"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75933 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75933"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75934 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75934"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75935 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75935"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75936 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75936"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct epinions75937 final : public EpinionsBase {
   std::string_view name() const override { return "epinions75937"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
inline factDB::BaseTable* create_epinions75888() { return new epinions75888; }
inline factDB::BaseTable* create_epinions75889() { return new epinions75889; }
inline factDB::BaseTable* create_epinions75890() { return new epinions75890; }
inline factDB::BaseTable* create_epinions75891() { return new epinions75891; }
inline factDB::BaseTable* create_epinions75892() { return new epinions75892; }
inline factDB::BaseTable* create_epinions75893() { return new epinions75893; }
inline factDB::BaseTable* create_epinions75894() { return new epinions75894; }
inline factDB::BaseTable* create_epinions75895() { return new epinions75895; }
inline factDB::BaseTable* create_epinions75896() { return new epinions75896; }
inline factDB::BaseTable* create_epinions75897() { return new epinions75897; }
inline factDB::BaseTable* create_epinions75898() { return new epinions75898; }
inline factDB::BaseTable* create_epinions75899() { return new epinions75899; }
inline factDB::BaseTable* create_epinions75900() { return new epinions75900; }
inline factDB::BaseTable* create_epinions75901() { return new epinions75901; }
inline factDB::BaseTable* create_epinions75902() { return new epinions75902; }
inline factDB::BaseTable* create_epinions75903() { return new epinions75903; }
inline factDB::BaseTable* create_epinions75904() { return new epinions75904; }
inline factDB::BaseTable* create_epinions75905() { return new epinions75905; }
inline factDB::BaseTable* create_epinions75906() { return new epinions75906; }
inline factDB::BaseTable* create_epinions75907() { return new epinions75907; }
inline factDB::BaseTable* create_epinions75908() { return new epinions75908; }
inline factDB::BaseTable* create_epinions75909() { return new epinions75909; }
inline factDB::BaseTable* create_epinions75910() { return new epinions75910; }
inline factDB::BaseTable* create_epinions75911() { return new epinions75911; }
inline factDB::BaseTable* create_epinions75912() { return new epinions75912; }
inline factDB::BaseTable* create_epinions75913() { return new epinions75913; }
inline factDB::BaseTable* create_epinions75914() { return new epinions75914; }
inline factDB::BaseTable* create_epinions75915() { return new epinions75915; }
inline factDB::BaseTable* create_epinions75916() { return new epinions75916; }
inline factDB::BaseTable* create_epinions75917() { return new epinions75917; }
inline factDB::BaseTable* create_epinions75918() { return new epinions75918; }
inline factDB::BaseTable* create_epinions75919() { return new epinions75919; }
inline factDB::BaseTable* create_epinions75920() { return new epinions75920; }
inline factDB::BaseTable* create_epinions75921() { return new epinions75921; }
inline factDB::BaseTable* create_epinions75922() { return new epinions75922; }
inline factDB::BaseTable* create_epinions75923() { return new epinions75923; }
inline factDB::BaseTable* create_epinions75924() { return new epinions75924; }
inline factDB::BaseTable* create_epinions75925() { return new epinions75925; }
inline factDB::BaseTable* create_epinions75926() { return new epinions75926; }
inline factDB::BaseTable* create_epinions75927() { return new epinions75927; }
inline factDB::BaseTable* create_epinions75928() { return new epinions75928; }
inline factDB::BaseTable* create_epinions75929() { return new epinions75929; }
inline factDB::BaseTable* create_epinions75930() { return new epinions75930; }
inline factDB::BaseTable* create_epinions75931() { return new epinions75931; }
inline factDB::BaseTable* create_epinions75932() { return new epinions75932; }
inline factDB::BaseTable* create_epinions75933() { return new epinions75933; }
inline factDB::BaseTable* create_epinions75934() { return new epinions75934; }
inline factDB::BaseTable* create_epinions75935() { return new epinions75935; }
inline factDB::BaseTable* create_epinions75936() { return new epinions75936; }
inline factDB::BaseTable* create_epinions75937() { return new epinions75937; }
// ---------------------------------------------------------------------------------------------------
inline void destroy_epinions75888(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75889(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75890(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75891(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75892(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75893(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75894(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75895(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75896(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75897(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75898(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75899(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75900(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75901(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75902(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75903(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75904(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75905(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75906(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75907(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75908(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75909(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75910(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75911(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75912(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75913(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75914(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75915(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75916(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75917(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75918(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75919(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75920(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75921(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75922(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75923(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75924(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75925(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75926(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75927(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75928(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75929(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75930(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75931(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75932(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75933(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75934(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75935(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75936(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_epinions75937(const factDB::BaseTable* ptr) { delete ptr; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
