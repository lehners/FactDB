#ifndef H_factdb_algebra_translator_FactorizedTableTranslator
#define H_factdb_algebra_translator_FactorizedTableTranslator
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/queryc/NewFileWriter.hpp"
#include <limits>
#include <set>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::translator {
// ---------------------------------------------------------------------------------------------------
class FactorizedTableNode {
   static size_t globalId;

   private:
   size_t factorized_table_node_id = globalId++;
   size_t iter_idx = 0;
   size_t subiterator_idx = 0;
   bool require_size_ = false;

   OrderedIUSet ius; // order is relevant
   std::vector<FactorizedTableNode> children = {};
   FactorizedTableNode* parent = nullptr;
   FactorizedTableNode* inserted_node = nullptr;

   FWContainer iter = fw::fmt("iter{}", iter_idx);
   FWContainer list_type = fw::fmt("ListType{}", iter_idx);
   FWContainer iterator_type = fw::lc("Iterator1");

   std::vector<std::vector<const IU*>> subiterators = {{}};

   void updateContainer();
   void setSubiteratorIdx(size_t i);

   [[nodiscard]] const std::vector<const IU*>& get_subiterator() const { return subiterators[subiterator_idx]; }
   [[nodiscard]] bool is_leaf() const { return children.empty(); }
   [[nodiscard]] bool contains_iu(const IU& iu) const { return ius.contains(&iu); }
   [[nodiscard]] bool containChildrenIu(const factDB::IU& iu) const;
   [[nodiscard]] bool is_required() const { return !get_subiterator().empty(); }
   [[nodiscard]] auto filteredChildren() const;
   [[nodiscard]] bool is_root() const { return parent == nullptr; }
   [[nodiscard]] FWContainer genListTypeStr(bool internal) const;

   public:
   FWContainer class_name;

   explicit FactorizedTableNode(size_t parent_idx = globalId++) : factorized_table_node_id(parent_idx), class_name("FactorizedTable", factorized_table_node_id) {}
   FactorizedTableNode(const FactorizedTableNode& node) : factorized_table_node_id(node.factorized_table_node_id), ius(node.ius), children(node.children), class_name("FactorizedTable", factorized_table_node_id){};
   //FactorizedTableNode(const FactorizedTableNode&) = delete;
   FactorizedTableNode operator=(const FactorizedTableNode&) = delete;
   FactorizedTableNode(FactorizedTableNode&&) = default;
   FactorizedTableNode& operator=(FactorizedTableNode&&) = default;
   ~FactorizedTableNode() {
      if (inserted_node != nullptr) delete inserted_node;
   }

   void require_size() { require_size_ = true; }
   void set_factorized_table_node_id(size_t new_idx) {
      factorized_table_node_id = new_idx;
      class_name = fw::lc("FactorizedTable", factorized_table_node_id);
   }
   size_t setIteratorIdxs(size_t idx = 1, FactorizedTableNode* parentPtr = nullptr);

   void add_iu(const IU& iu) { ius.push_back(&iu); }
   FactorizedTableNode& addChild(const factDB::IU& iu);
   FactorizedTableNode& addChild(const FactorizedTableNode& node);
   FactorizedTableNode& addChild(const FactorizedTableNode&& node);

   void genStruct(FileWriter& out);
   void genSize(FileWriter& out, size_t parentIdx = 0) const;
   void genPrinter(FileWriter& out, size_t parentIdx = 0) const;
   void genVariables(FileWriter& out) const;
   void genInserts(FileWriter& out, size_t parentIdx = 0) const;

   void genListType(FileWriter& out) const;
   void genIteratorTypeList(FileWriter& out, bool reference) const;

   void genIterator(FileWriter& out) const;

   void genIteratorEnd(FileWriter& out, const FWContainer& parentIter, const FWContainer& prefix = fw::nop()) const;
   void genIteratorIsLast(FileWriter& out) const;
   void genIteratorIncrement(FileWriter& iterParam, const FWContainer& prevUpdates) const;
   void genIteratorDereference(FileWriter& out) const;
   void genIteratorEquality(FileWriter& out, bool equal) const;
   void genIteratorInstances(FileWriter& out) const;
   void genIteratorInstancesUpdate(
      FileWriter& out, const FWContainer& parentIter, bool needSelf = true, const FWContainer& iterPrefix = fw::nop(), const FWContainer& prefix = fw::nop(),
      const std::function<FWContainer(const FWContainer&)>& suffix = [](const FWContainer&) { return fw::nop(); },
      size_t cutoff = std::numeric_limits<size_t>::max()) const;
   void genIteratorInsert(const std::vector<IU*>& toInsert, FactorizedTableNode& newNode, FactorizedTableNode& mergedNode) const;
   FactorizedTableNode& genIteratorInsert(const std::vector<factDB::IU*>& toInsert);
   void genIteratorInsertFunction(FileWriter& out, const FactorizedTableNode& other) const;

   void getRequiredChildren(std::vector<const FactorizedTableNode*>& todo) const;
   void genIteratorInsertFunAssignmentHelper(FileWriter& out, size_t& offsetIus, size_t& offsetChildren, FWContainer& lcRef, const std::string& op1, const std::string& op2) const;

   static std::vector<std::string> get_required_headers() {
      return {"factDB/infra/list/ChunkedList.hpp",
              "factDB/infra/Types.hpp",
              "factDB/queryc/NewFileWriter.hpp"};
   }

   template <typename... IUs>
   void set_dereference_order(const IUs&... iu_order);
   template <typename... IUs>
   std::pair<FWContainer, FWContainer> add_subiterator(const IUs&... iu_order);

   template <typename... T>
   FWContainer push_back(const std::string& table, const IU& iu, T... var);
   template <typename... T>
   FWContainer push_back(const std::string& table, const std::string& handle, const IU& iu, T... var);

   [[nodiscard]] FWContainer genListTypeStr(const IU& iu) const;
   [[nodiscard]] FWContainer gen_iterator_type_list(bool reference) const {
      return fw::func([&, reference](FileWriter& out) { genIteratorTypeList(out, reference); });
   }
   [[nodiscard]] FWContainer gen_iterator_type(bool reference) const {
      return fw::func([&, reference](FileWriter& out) { out << fw::fmt("std::tuple<{}>", gen_iterator_type_list(reference)); });
   }
};
// ---------------------------------------------------------------------------------------------------
template <typename... IUs>
void FactorizedTableNode::set_dereference_order(const IUs&... iu_order) {
   if (is_root()) {
      subiterators[0] = OrderedIUSet{&iu_order...};
      setIteratorIdxs();
      for (auto& child : children)
         child.set_dereference_order(iu_order...);
   } else {
      for (auto& child : children) {
         child.set_dereference_order(iu_order...);
         subiterators[0].insert(subiterators.back().end(), child.subiterators.back().begin(), child.subiterators.back().end());
      }
      [[maybe_unused]] auto insert_iu = [&](const IU* iu) { if (ius.contains(iu)) subiterators.back().push_back(iu); };
      (insert_iu(&iu_order), ...);
   }
}
// ---------------------------------------------------------------------------------------------------
template <typename... IUs>
std::pair<FWContainer, FWContainer> FactorizedTableNode::add_subiterator(const IUs&... iu_order) {
   if (is_root()) {
      subiterators.push_back({&iu_order...});
      for (auto& child : children)
         child.add_subiterator(iu_order...);
      return std::make_pair(fw::lc("begin", subiterators.size() - 1), fw::lc("end", subiterators.size() - 1));
   } else {
      subiterators.emplace_back();
      for (auto& child : children) {
         child.add_subiterator(iu_order...);
         subiterators.back().insert(subiterators.back().end(), child.subiterators.back().begin(), child.subiterators.back().end());
      }
      auto insert_iu = [&](const IU* iu) { if (ius.contains(iu)) subiterators.back().push_back(iu); };
      (insert_iu(&iu_order), ...);
      return {fw::nop(), fw::nop()}; // return value of inner nodes is never used
   }
}
// ---------------------------------------------------------------------------------------------------
template <typename... T>
FWContainer FactorizedTableNode::push_back(const std::string& table, const std::string& handle, const IU& iu, T... vars) {
   if (handle.empty()) return push_back(table, iu, vars...);
   if (contains_iu(iu)) {
      assert(sizeof...(T) == ius.size());
      auto iu_iter = ius.begin();
      return fw::lc(table, ".push_back(", handle, ", ", class_name, "::", &iu, fw::lc(", ", (*iu_iter++)->type, "(", vars, ")")..., ");");
   }
   for (auto& child : children) {
      if (child.contains_iu(iu) || child.containChildrenIu(iu)) {
         return child.push_back(table, handle, iu, vars...);
      }
   }
   assert(false && "the specified IU was not found");
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
template <typename... T>
FWContainer FactorizedTableNode::push_back(const std::string& table, const IU& iu, T... vars) {
   assert(ius.contains(&iu));
   assert(sizeof...(T) == ius.size());

   auto iu_iter = ius.begin();
   return fw::lc(table, ".push_back(", class_name, "::", &iu, fw::lc(", ", (*iu_iter++)->type, "(", vars, ")")..., ");");
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::translator
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_algebra_translator_FactorizedTableTranslator