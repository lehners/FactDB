#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
struct WatdivBase : public BaseTable {
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

   ~WatdivBase() override = default;
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;

   protected:
   static schemac::Table genSchema(uint32_t id, std::string name);
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052572 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052572"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052573 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052573"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052574 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052574"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052575 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052575"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052576 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052576"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052577 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052577"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052578 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052578"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052579 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052579"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052580 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052580"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052581 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052581"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052582 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052582"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052583 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052583"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052584 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052584"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052585 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052585"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052586 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052586"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052587 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052587"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052588 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052588"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052589 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052589"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052590 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052590"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052591 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052591"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052592 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052592"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052593 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052593"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052594 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052594"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052595 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052595"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052596 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052596"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052597 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052597"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052598 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052598"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052599 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052599"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052600 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052600"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052601 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052601"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052602 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052602"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052603 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052603"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052604 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052604"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052605 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052605"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052606 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052606"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052607 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052607"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052608 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052608"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052609 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052609"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052610 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052610"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052611 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052611"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052612 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052612"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052613 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052613"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052614 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052614"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052615 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052615"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052616 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052616"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052617 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052617"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052618 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052618"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052619 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052619"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052620 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052620"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052621 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052621"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052622 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052622"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052623 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052623"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052624 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052624"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052625 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052625"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052626 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052626"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052627 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052627"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052628 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052628"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052629 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052629"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052630 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052630"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052631 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052631"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052632 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052632"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052633 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052633"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052634 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052634"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052635 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052635"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052636 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052636"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052637 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052637"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052638 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052638"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052639 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052639"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052640 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052640"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052641 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052641"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052642 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052642"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052643 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052643"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052644 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052644"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052645 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052645"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052646 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052646"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052647 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052647"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052648 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052648"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052649 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052649"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052650 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052650"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052651 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052651"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052652 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052652"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052653 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052653"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052654 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052654"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052655 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052655"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052656 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052656"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct watdiv1052657 final : public WatdivBase {
   std::string_view name() const override { return "watdiv1052657"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
inline factDB::BaseTable* create_watdiv1052572() { return new watdiv1052572; }
inline factDB::BaseTable* create_watdiv1052573() { return new watdiv1052573; }
inline factDB::BaseTable* create_watdiv1052574() { return new watdiv1052574; }
inline factDB::BaseTable* create_watdiv1052575() { return new watdiv1052575; }
inline factDB::BaseTable* create_watdiv1052576() { return new watdiv1052576; }
inline factDB::BaseTable* create_watdiv1052577() { return new watdiv1052577; }
inline factDB::BaseTable* create_watdiv1052578() { return new watdiv1052578; }
inline factDB::BaseTable* create_watdiv1052579() { return new watdiv1052579; }
inline factDB::BaseTable* create_watdiv1052580() { return new watdiv1052580; }
inline factDB::BaseTable* create_watdiv1052581() { return new watdiv1052581; }
inline factDB::BaseTable* create_watdiv1052582() { return new watdiv1052582; }
inline factDB::BaseTable* create_watdiv1052583() { return new watdiv1052583; }
inline factDB::BaseTable* create_watdiv1052584() { return new watdiv1052584; }
inline factDB::BaseTable* create_watdiv1052585() { return new watdiv1052585; }
inline factDB::BaseTable* create_watdiv1052586() { return new watdiv1052586; }
inline factDB::BaseTable* create_watdiv1052587() { return new watdiv1052587; }
inline factDB::BaseTable* create_watdiv1052588() { return new watdiv1052588; }
inline factDB::BaseTable* create_watdiv1052589() { return new watdiv1052589; }
inline factDB::BaseTable* create_watdiv1052590() { return new watdiv1052590; }
inline factDB::BaseTable* create_watdiv1052591() { return new watdiv1052591; }
inline factDB::BaseTable* create_watdiv1052592() { return new watdiv1052592; }
inline factDB::BaseTable* create_watdiv1052593() { return new watdiv1052593; }
inline factDB::BaseTable* create_watdiv1052594() { return new watdiv1052594; }
inline factDB::BaseTable* create_watdiv1052595() { return new watdiv1052595; }
inline factDB::BaseTable* create_watdiv1052596() { return new watdiv1052596; }
inline factDB::BaseTable* create_watdiv1052597() { return new watdiv1052597; }
inline factDB::BaseTable* create_watdiv1052598() { return new watdiv1052598; }
inline factDB::BaseTable* create_watdiv1052599() { return new watdiv1052599; }
inline factDB::BaseTable* create_watdiv1052600() { return new watdiv1052600; }
inline factDB::BaseTable* create_watdiv1052601() { return new watdiv1052601; }
inline factDB::BaseTable* create_watdiv1052602() { return new watdiv1052602; }
inline factDB::BaseTable* create_watdiv1052603() { return new watdiv1052603; }
inline factDB::BaseTable* create_watdiv1052604() { return new watdiv1052604; }
inline factDB::BaseTable* create_watdiv1052605() { return new watdiv1052605; }
inline factDB::BaseTable* create_watdiv1052606() { return new watdiv1052606; }
inline factDB::BaseTable* create_watdiv1052607() { return new watdiv1052607; }
inline factDB::BaseTable* create_watdiv1052608() { return new watdiv1052608; }
inline factDB::BaseTable* create_watdiv1052609() { return new watdiv1052609; }
inline factDB::BaseTable* create_watdiv1052610() { return new watdiv1052610; }
inline factDB::BaseTable* create_watdiv1052611() { return new watdiv1052611; }
inline factDB::BaseTable* create_watdiv1052612() { return new watdiv1052612; }
inline factDB::BaseTable* create_watdiv1052613() { return new watdiv1052613; }
inline factDB::BaseTable* create_watdiv1052614() { return new watdiv1052614; }
inline factDB::BaseTable* create_watdiv1052615() { return new watdiv1052615; }
inline factDB::BaseTable* create_watdiv1052616() { return new watdiv1052616; }
inline factDB::BaseTable* create_watdiv1052617() { return new watdiv1052617; }
inline factDB::BaseTable* create_watdiv1052618() { return new watdiv1052618; }
inline factDB::BaseTable* create_watdiv1052619() { return new watdiv1052619; }
inline factDB::BaseTable* create_watdiv1052620() { return new watdiv1052620; }
inline factDB::BaseTable* create_watdiv1052621() { return new watdiv1052621; }
inline factDB::BaseTable* create_watdiv1052622() { return new watdiv1052622; }
inline factDB::BaseTable* create_watdiv1052623() { return new watdiv1052623; }
inline factDB::BaseTable* create_watdiv1052624() { return new watdiv1052624; }
inline factDB::BaseTable* create_watdiv1052625() { return new watdiv1052625; }
inline factDB::BaseTable* create_watdiv1052626() { return new watdiv1052626; }
inline factDB::BaseTable* create_watdiv1052627() { return new watdiv1052627; }
inline factDB::BaseTable* create_watdiv1052628() { return new watdiv1052628; }
inline factDB::BaseTable* create_watdiv1052629() { return new watdiv1052629; }
inline factDB::BaseTable* create_watdiv1052630() { return new watdiv1052630; }
inline factDB::BaseTable* create_watdiv1052631() { return new watdiv1052631; }
inline factDB::BaseTable* create_watdiv1052632() { return new watdiv1052632; }
inline factDB::BaseTable* create_watdiv1052633() { return new watdiv1052633; }
inline factDB::BaseTable* create_watdiv1052634() { return new watdiv1052634; }
inline factDB::BaseTable* create_watdiv1052635() { return new watdiv1052635; }
inline factDB::BaseTable* create_watdiv1052636() { return new watdiv1052636; }
inline factDB::BaseTable* create_watdiv1052637() { return new watdiv1052637; }
inline factDB::BaseTable* create_watdiv1052638() { return new watdiv1052638; }
inline factDB::BaseTable* create_watdiv1052639() { return new watdiv1052639; }
inline factDB::BaseTable* create_watdiv1052640() { return new watdiv1052640; }
inline factDB::BaseTable* create_watdiv1052641() { return new watdiv1052641; }
inline factDB::BaseTable* create_watdiv1052642() { return new watdiv1052642; }
inline factDB::BaseTable* create_watdiv1052643() { return new watdiv1052643; }
inline factDB::BaseTable* create_watdiv1052644() { return new watdiv1052644; }
inline factDB::BaseTable* create_watdiv1052645() { return new watdiv1052645; }
inline factDB::BaseTable* create_watdiv1052646() { return new watdiv1052646; }
inline factDB::BaseTable* create_watdiv1052647() { return new watdiv1052647; }
inline factDB::BaseTable* create_watdiv1052648() { return new watdiv1052648; }
inline factDB::BaseTable* create_watdiv1052649() { return new watdiv1052649; }
inline factDB::BaseTable* create_watdiv1052650() { return new watdiv1052650; }
inline factDB::BaseTable* create_watdiv1052651() { return new watdiv1052651; }
inline factDB::BaseTable* create_watdiv1052652() { return new watdiv1052652; }
inline factDB::BaseTable* create_watdiv1052653() { return new watdiv1052653; }
inline factDB::BaseTable* create_watdiv1052654() { return new watdiv1052654; }
inline factDB::BaseTable* create_watdiv1052655() { return new watdiv1052655; }
inline factDB::BaseTable* create_watdiv1052656() { return new watdiv1052656; }
inline factDB::BaseTable* create_watdiv1052657() { return new watdiv1052657; }
// ---------------------------------------------------------------------------------------------------
inline void destroy_watdiv1052572(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052573(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052574(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052575(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052576(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052577(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052578(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052579(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052580(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052581(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052582(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052583(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052584(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052585(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052586(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052587(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052588(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052589(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052590(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052591(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052592(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052593(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052594(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052595(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052596(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052597(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052598(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052599(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052600(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052601(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052602(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052603(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052604(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052605(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052606(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052607(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052608(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052609(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052610(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052611(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052612(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052613(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052614(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052615(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052616(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052617(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052618(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052619(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052620(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052621(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052622(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052623(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052624(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052625(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052626(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052627(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052628(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052629(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052630(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052631(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052632(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052633(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052634(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052635(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052636(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052637(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052638(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052639(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052640(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052641(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052642(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052643(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052644(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052645(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052646(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052647(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052648(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052649(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052650(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052651(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052652(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052653(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052654(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052655(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052656(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_watdiv1052657(const factDB::BaseTable* ptr) { delete ptr; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------