#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
struct HetioBase : public BaseTable {
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

   ~HetioBase() override = default;
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;

   protected:
   static schemac::Table genSchema(uint32_t id, std::string name);
};
// ---------------------------------------------------------------------------------------------------
struct hetio45159 final : public HetioBase {
   std::string_view name() const override { return "hetio45159"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45160 final : public HetioBase {
   std::string_view name() const override { return "hetio45160"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45161 final : public HetioBase {
   std::string_view name() const override { return "hetio45161"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45162 final : public HetioBase {
   std::string_view name() const override { return "hetio45162"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45163 final : public HetioBase {
   std::string_view name() const override { return "hetio45163"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45164 final : public HetioBase {
   std::string_view name() const override { return "hetio45164"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45165 final : public HetioBase {
   std::string_view name() const override { return "hetio45165"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45166 final : public HetioBase {
   std::string_view name() const override { return "hetio45166"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45167 final : public HetioBase {
   std::string_view name() const override { return "hetio45167"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45168 final : public HetioBase {
   std::string_view name() const override { return "hetio45168"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45169 final : public HetioBase {
   std::string_view name() const override { return "hetio45169"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45170 final : public HetioBase {
   std::string_view name() const override { return "hetio45170"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45171 final : public HetioBase {
   std::string_view name() const override { return "hetio45171"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45172 final : public HetioBase {
   std::string_view name() const override { return "hetio45172"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45173 final : public HetioBase {
   std::string_view name() const override { return "hetio45173"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45174 final : public HetioBase {
   std::string_view name() const override { return "hetio45174"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45175 final : public HetioBase {
   std::string_view name() const override { return "hetio45175"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45176 final : public HetioBase {
   std::string_view name() const override { return "hetio45176"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45177 final : public HetioBase {
   std::string_view name() const override { return "hetio45177"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45178 final : public HetioBase {
   std::string_view name() const override { return "hetio45178"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45179 final : public HetioBase {
   std::string_view name() const override { return "hetio45179"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45180 final : public HetioBase {
   std::string_view name() const override { return "hetio45180"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45181 final : public HetioBase {
   std::string_view name() const override { return "hetio45181"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct hetio45182 final : public HetioBase {
   std::string_view name() const override { return "hetio45182"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
inline factDB::BaseTable* create_hetio45159() { return new hetio45159; }
inline factDB::BaseTable* create_hetio45160() { return new hetio45160; }
inline factDB::BaseTable* create_hetio45161() { return new hetio45161; }
inline factDB::BaseTable* create_hetio45162() { return new hetio45162; }
inline factDB::BaseTable* create_hetio45163() { return new hetio45163; }
inline factDB::BaseTable* create_hetio45164() { return new hetio45164; }
inline factDB::BaseTable* create_hetio45165() { return new hetio45165; }
inline factDB::BaseTable* create_hetio45166() { return new hetio45166; }
inline factDB::BaseTable* create_hetio45167() { return new hetio45167; }
inline factDB::BaseTable* create_hetio45168() { return new hetio45168; }
inline factDB::BaseTable* create_hetio45169() { return new hetio45169; }
inline factDB::BaseTable* create_hetio45170() { return new hetio45170; }
inline factDB::BaseTable* create_hetio45171() { return new hetio45171; }
inline factDB::BaseTable* create_hetio45172() { return new hetio45172; }
inline factDB::BaseTable* create_hetio45173() { return new hetio45173; }
inline factDB::BaseTable* create_hetio45174() { return new hetio45174; }
inline factDB::BaseTable* create_hetio45175() { return new hetio45175; }
inline factDB::BaseTable* create_hetio45176() { return new hetio45176; }
inline factDB::BaseTable* create_hetio45177() { return new hetio45177; }
inline factDB::BaseTable* create_hetio45178() { return new hetio45178; }
inline factDB::BaseTable* create_hetio45179() { return new hetio45179; }
inline factDB::BaseTable* create_hetio45180() { return new hetio45180; }
inline factDB::BaseTable* create_hetio45181() { return new hetio45181; }
inline factDB::BaseTable* create_hetio45182() { return new hetio45182; }
// ---------------------------------------------------------------------------------------------------
inline void destroy_hetio45159(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45160(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45161(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45162(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45163(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45164(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45165(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45166(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45167(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45168(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45169(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45170(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45171(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45172(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45173(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45174(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45175(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45176(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45177(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45178(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45179(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45180(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45181(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_hetio45182(const factDB::BaseTable* ptr) { delete ptr; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
