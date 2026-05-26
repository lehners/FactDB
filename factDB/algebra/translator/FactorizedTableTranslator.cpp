// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/translator/FactorizedTableTranslator.hpp"
#include "factDB/infra/util/ranges.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::translator {
// ---------------------------------------------------------------------------------------------------
size_t FactorizedTableNode::globalId = 0;
// ---------------------------------------------------------------------------------------------------
auto FactorizedTableNode::filteredChildren() const {
   return children | views::filter([](const FactorizedTableNode& child) { return child.is_required(); });
}
// ---------------------------------------------------------------------------------------------------
bool FactorizedTableNode::containChildrenIu(const factDB::IU& iu) const {
   for (const auto& child : children) {
      if (child.contains_iu(iu) || child.containChildrenIu(iu))
         return true;
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
FactorizedTableNode& FactorizedTableNode::addChild(const factDB::IU& iu) {
   auto& child = children.emplace_back(factorized_table_node_id);
   child.ius.push_back(&iu);
   return child;
}
// ---------------------------------------------------------------------------------------------------
FactorizedTableNode& FactorizedTableNode::addChild(const FactorizedTableNode& node) {
   return children.emplace_back(node);
}
// ---------------------------------------------------------------------------------------------------
FactorizedTableNode& FactorizedTableNode::addChild(const FactorizedTableNode&& node) {
   return children.emplace_back(std::move(node));
}
// ---------------------------------------------------------------------------------------------------
size_t FactorizedTableNode::setIteratorIdxs(size_t idx, FactorizedTableNode* parentPtr) {
   iter_idx = idx;
   parent = parentPtr;
   for (auto& child : children)
      idx = child.setIteratorIdxs(++idx, this);
   updateContainer();
   return idx;
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::updateContainer() {
   iter = fw::fmt("iter{}", iter_idx);
   list_type = fw::fmt("ListType{}", iter_idx);
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::setSubiteratorIdx(size_t i) {
   subiterator_idx = i;
   iterator_type = fw::fmt("Iterator{}", i);
   for (auto& c : children)
      c.setSubiteratorIdx(i);
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genStruct(FileWriter& out) {
   if (inserted_node) {
      inserted_node->genStruct(out);
   } else {
      out << "using namespace factDB;" << fw::endl();
   }

   out << "struct " << class_name << " {" << fw::endl();
   genListType(out);
   out << fw::endl()
       << list_type << " cl;" << fw::endl()
       << fw::endl();
   if (require_size_)
      genSize(out);
   // genPrinter(out);
   genInserts(out);
   for (size_t i = 0; i < subiterators.size(); i++) {
      setSubiteratorIdx(i);
      genIterator(out);
      if (i != 0) {
         out << iterator_type << " begin" << i << "() { return " << iterator_type << "::begin(*this); }" << fw::endl()
             << iterator_type << " end" << i << "() { return " << iterator_type << "::end(*this); }" << fw::endl()
             << fw::endl();
      }
   }
   out << "Iterator0 begin()  { return Iterator0::begin(*this); }" << fw::endl()
       << "Iterator0 end()  { return Iterator0::end(*this); }" << fw::endl()
       << "};" << fw::endl()
       << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genSize(FileWriter& out, size_t parentIdx) const {
   auto vSum = fw::lc("sum_", iter_idx);
   auto vSize = fw::lc("size", iter_idx);
   auto vIter = fw::lc("iter_", iter_idx);
   auto vparentIter = is_root() ? fw::nop() : fw::lc("iter_", parent->iter_idx);
   auto vparentSizeOp = is_root() ? fw::lc("return ") : fw::lc("size", parent->iter_idx, " *= ");
   auto vCl = is_root() ? "cl" : fw::get(parentIdx, vparentIter);

   if (is_root())
      out << "size_t size() {" << fw::endl();

   if (children.empty()) {
      out << vparentSizeOp << vCl << ".size();" << fw::endl();
   } else {
      out << "size_t " << vSum << " = 0;" << fw::endl()
          << "for (auto& " << vIter << " : " << vCl << ") {" << fw::endl()
          << "  size_t " << vSize << " = 1;" << fw::endl();
      for (const auto&& [childIdx, child] : children | views::enumerate)
         child.genSize(out, childIdx + ius.size());
      out << vSum << " += " << vSize << ";" << fw::endl()
          << "}" << fw::endl()
          << vparentSizeOp << vSum << fw::sendl();
   }

   if (is_root())
      out << "}" << fw::endl()
          << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genPrinter(FileWriter& out, size_t parentIdx) const {
   auto vIter = fw::lc("iter_{}", iter_idx), vFirst = fw::lc("first_", iter_idx);
   out.setSeparator(" << ", false, false);
   auto vParentIdx = fw::lc("iter_", is_root() ? 0 : parent->iter_idx);
   auto vParent = is_root() ? "cl" : fw::get(parentIdx, vParentIdx);

   if (is_root())
      out << "void print(FileWriter& out) const {" << fw::endl();

   out << "out" << fw::separator() << fw::str("{ ") << fw::sendl()
       << "bool " << vFirst << " = true;" << fw::endl()
       << "for (auto& " << vIter << " : " << vParent << ") {" << fw::endl()
       << "if (!" << vFirst << ") out" << fw::separator() << fw::str(", ") << fw::sendl()
       << vFirst << " = false;" << fw::endl()
       << "out" << (ius.size() != 1 ? fw::lc(fw::separator(), fw::str("(")) : fw::nop()) << fw::separator()
       << (ius | views::enumerate | views::transform([&](const auto& eIu) { return fw::lc(fw::condition(std::get<0>(eIu), "\", \" << "), fw::get(std::get<0>(eIu), vIter), ".value"); }))
       << (ius.size() != 1 ? fw::lc(fw::separator(), fw::str(")")) : fw::nop()) << fw::sendl();

   bool onlyLeafSeen = true;
   for (const auto& [idx, child] : views::enumerate(children)) {
      out << "out" << fw::separator() << fw::str(" × ") << (child.is_leaf() ? fw::nop() : fw::lc(fw::separator(), "fw::endl()")) << fw::sendl();
      child.genPrinter(out, idx + ius.size());
      onlyLeafSeen &= child.is_leaf();
   }
   out << "}" << fw::endl()
       << "out" << (onlyLeafSeen ? fw::nop() : fw::lc(fw::separator(), "fw::endl()")) << fw::separator() << fw::str(" }") << fw::sendl()
       << (is_root() ? "}" : "") << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genInserts(FileWriter& out, size_t parentIdx) const {
   auto handle = fw::lc("handle"), parentHandle = fw::lc("parentHandle");
   auto pbEnum = fw::lc("PB_enum", iter_idx);
   out.setSeparator(", ");

   out << "enum " << pbEnum << " { " << ius << " };" << fw::endl()
       << genListTypeStr(true) << "& push_back(";
   if (!is_root())
      out << parent->genListTypeStr(true) << "& " << parentHandle << fw::separator();

   out << pbEnum << fw::separator() << (ius | views::transform([](const IU* iu) { return fw::lc("const ", iu->type.toString(), "& val_", *iu); })) << ") {" << fw::endl()
       << "auto& " << handle << " = *" << (is_root() ? "cl" : fw::get(parentIdx, parentHandle)) << ".emplace_back();" << fw::endl();

   out.setSeparator("");
   out << (ius | views::enumerate | views::transform([&](auto eIu) { return fw::lc(fw::get(std::get<0>(eIu), handle), " = val_", std::get<1>(eIu), fw::sendl()); }))
       << "return " << handle << fw::sendl()
       << "}" << fw::endl()
       << fw::endl();

   for (const auto&& [idx, child] : children | views::enumerate) {
      child.genInserts(out, ius.size() + idx);
   }
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genListType(FileWriter& out) const {
   for (auto& child : children)
      child.genListType(out);
   out.setSeparator(", ");
   out << fw::fmt("using {} = /*{}*/ factDB::infra::list::OwningChunkedList<std::tuple<", list_type, **ius.begin())
       << (ius | views::transform([](const IU* iu) { return iu->type.toString(); }))
       << fw::separator(!children.empty()) << (children | views::transform([](const auto& child) { return fw::lc(child.list_type); }))
       << ">>;" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genVariables(FileWriter& out) const {
   out.setSeparator("", true);
   out << (ius | views::enumerate | views::transform([&](auto eIu) { return fw::lc("auto& ", std::get<1>(eIu), " = ", fw::get(std::get<0>(eIu), iter.dereference()), fw::sendl()); }));

   for (const auto& child : filteredChildren())
      child.genVariables(out);
}
// ---------------------------------------------------------------------------------------------------
FWContainer FactorizedTableNode::genListTypeStr(bool internal) const {
   if (internal)
      return fw::lc(list_type, "::ElementType");
   else
      return fw::lc(class_name, "::", list_type, "::ElementType");
}
// ---------------------------------------------------------------------------------------------------
FWContainer FactorizedTableNode::genListTypeStr(const IU& iu) const {
   if (contains_iu(iu)) {
      return genListTypeStr(false);
   }
   for (const auto& child : children) {
      if (child.contains_iu(iu) || child.containChildrenIu(iu)) {
         return child.genListTypeStr(false);
      }
   }
   assert(false && "iu not contained in the table");
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIterator(FileWriter& out) const {
   out << "struct " << iterator_type << " {" << fw::endl()
       << "public:" << fw::endl()
       << "using differenceType = std::ptrdiff_t;" << fw::endl()
       << "using reference = " << gen_iterator_type(true) << ";" << fw::endl()
       << "using pointer = " << gen_iterator_type(false) << "*;" << fw::endl()
       << "using iteratorCategory = std::forward_iterator_tag;" << fw::endl()
       << fw::endl()
       << "private:" << fw::endl();
   genIteratorInstances(out);
   out << fw::endl()
       << "public:" << fw::endl()
       << "static " << iterator_type << " begin(" << class_name << "& t) {" << fw::endl()
       << iterator_type << " i;" << fw::endl();
   genIteratorInstancesUpdate(out, "t.cl", true, "i.");
   out << "  return i;" << fw::endl()
       << "}" << fw::endl()
       << fw::endl()
       << "static " << iterator_type << " end(" << class_name << "& t) {" << fw::endl()
       << iterator_type << " i;" << fw::endl();
   genIteratorEnd(out, "t.cl", "i.");
   out << "  return i;" << fw::endl()
       << "}" << fw::endl()
       << fw::endl()
       << "bool isLast() {" << fw::endl();
   genIteratorIsLast(out);
   out << "}" << fw::endl()
       << iterator_type << "& operator++() {" << fw::endl();
   genIteratorIncrement(out, fw::nop());
   out << "  return *this;" << fw::endl()
       << "}" << fw::endl()
       << fw::endl();
   genIteratorDereference(out);
   out << fw::endl()
       << "bool operator==(const " << iterator_type << "& other) const { return ";
   genIteratorEquality(out, true);
   out << "; }" << fw::endl()
       << "bool operator!=(const " << iterator_type << "& other) const { return ";
   genIteratorEquality(out, false);
   out << "; }" << fw::endl()
       << fw::endl();
   if (inserted_node != nullptr)
      genIteratorInsertFunction(out, *inserted_node);
   out << "};" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorTypeList(FileWriter& out, bool reference) const {
   if (is_root()) out.setSeparator(", ");
   out << (get_subiterator() | views::transform([reference](const IU* iu) { return (iu->type.toString() + (reference ? "&" : "")); }));
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorInstances(FileWriter& out) const {
   out << list_type << "::Iterator " << iter << fw::sendl();
   for (auto& child : filteredChildren())
      child.genIteratorInstances(out);
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorEnd(FileWriter& out, const FWContainer& parentIter, const FWContainer& prefix) const { // NOLINT
   const FWContainer iterVar = fw::lc(prefix, iter);
   out << iterVar << " = " << (!children.empty() ? "--" : "") << parentIter << ".end();" << fw::endl();

   if (!children.empty()) {
      const size_t iusSize = ius.size();
      for (auto enumeratedChild : (children | views::enumerate)) {
         auto& child = std::get<1>(enumeratedChild);
         if (!child.is_required()) continue;
         child.genIteratorEnd(out, fw::lc(fw::get(iusSize + std::get<0>(enumeratedChild), iterVar.dereference())), prefix);
      }
      out << "++" << iterVar << fw::sendl();
   }
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorIsLast(FileWriter& out) const {
   if (is_root()) {
      out.setSeparator(" && ");
      out << "return ";
   }
   out << fw::separator() << iter << ".isLast()";
   for (const auto& child : filteredChildren())
      child.genIteratorIsLast(out);

   if (is_root())
      out << ";" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorIncrement(FileWriter& out, const FWContainer& prevUpdates) const {
   if (is_root())
      out << "bool last_iter_bool=false;" << fw::endl();
   for (const auto&& [idx, child] : children | views::enumerate) {
      if (!child.is_required()) continue;
      child.genIteratorIncrement(out, fw::func([&, idxI = idx](FileWriter& o) {
                                    o << prevUpdates;
                                    genIteratorInstancesUpdate(
                                       o, fw::nop(), false, fw::nop(), fw::nop(), [&](auto& iterParam) { return fw::lc(" last_iter_bool |= ", iterParam, ".isLast();"); }, idxI);
                                 }));
   }
   out << "for ( ;!(++" << iter << ").isLast(); last_iter_bool=false) {" << fw::endl()
       << prevUpdates;
   genIteratorInstancesUpdate(out, fw::nop(), false, fw::nop(), fw::nop(), [&](auto& iterParam) { return fw::lc(" last_iter_bool |= ", iterParam, ".isLast();"); });
   out << "if (!last_iter_bool) return *this;" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorEquality(FileWriter& out, bool equal) const {
   auto cmp = equal ? " == " : " != ";
   if (is_root())
      out.setSeparator(equal ? " && " : " || ");
   out << fw::separator() << iter << cmp << "other." << iter;
   for (const auto& child : filteredChildren())
      child.genIteratorEquality(out, equal);
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorInstancesUpdate(FileWriter& out, const FWContainer& parentIter, bool needSelf, const FWContainer& iterPrefix, const FWContainer& prefix, const std::function<FWContainer(const FWContainer&)>& suffix, size_t cutoff) const {
   const auto iterVar = fw::lc(iterPrefix, iter);

   if (needSelf)
      out << prefix << iterVar << " = " << parentIter << ".begin();" << suffix(iterVar) << fw::endl();
   const size_t iusSize = ius.size();
   for (auto enumeratedChild : (children | views::enumerate)) {
      auto& child = std::get<1>(enumeratedChild);
      if (static_cast<size_t>(std::get<0>(enumeratedChild)) == cutoff) break;
      if (!child.is_required()) continue;
      child.genIteratorInstancesUpdate(out, fw::lc(fw::get(iusSize + std::get<0>(enumeratedChild), iterVar.dereference())), true, iterPrefix, prefix, suffix);
   }
}
// ---------------------------------------------------------------------------------------------------
FactorizedTableNode& FactorizedTableNode::genIteratorInsert(const std::vector<factDB::IU*>& toInsert) {
   if (inserted_node) delete inserted_node;
   inserted_node = new FactorizedTableNode();
   FactorizedTableNode newMergedChild(inserted_node->factorized_table_node_id);
   genIteratorInsert(toInsert, *inserted_node, newMergedChild);
   return *inserted_node;
}

void FactorizedTableNode::genIteratorInsert(const std::vector<IU*>& toInsert, FactorizedTableNode& newNode, FactorizedTableNode& mergedNode) const {
   for (const IU* iu : ius)
      newNode.add_iu(*iu);
   std::vector<const FactorizedTableNode*> todos;
   for (const auto&& [childIdx, child] : children | views::enumerate) {
      if (child.is_required()) {
         //for (const IU* iu : child.ius)
         //   mergedNode.addChild(*iu);
         todos.push_back(&child);
      } else {
         newNode.addChild(child);
      }
   }

   FactorizedTableNode newMergedChild(newNode.factorized_table_node_id);
   // now check child of children...
   for (const FactorizedTableNode* t : todos) {
      t->genIteratorInsert(toInsert, mergedNode, newMergedChild);
   }
   if (!todos.empty()) {
      if (newMergedChild.children.empty() && newMergedChild.ius.empty())
         for (auto& toI : toInsert)
            newMergedChild.add_iu(*toI);
      mergedNode.addChild(std::move(newMergedChild));
   }

   if (is_root()) {
      if (mergedNode.children.empty() && mergedNode.ius.empty())
         for (auto& toI : toInsert)
            mergedNode.add_iu(*toI);
      newNode.addChild(std::move(mergedNode));
   }
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorInsertFunAssignmentHelper(FileWriter& out, size_t& offsetIus, size_t& offsetChildren, FWContainer& lcRef, const std::string& op1, const std::string& op2) const { // NOLINT
   for (const auto&& [idx, iu] : ius | views::enumerate) {
      out << fw::separator() << "std::get<" << offsetIus++ << ">(" << lcRef << ".back()) " << op1 << " std::get<" << idx << ">(*" << iter << ")";
   }

   for (const auto&& [idx, child] : children | views::enumerate) {
      if (child.is_required()) continue;
      out << fw::separator() << "std::get<" << offsetChildren++ << ">(" << lcRef << ".back()) " << op2 << " std::get<" << idx + ius.size() << ">(*" << iter << ")";
   }
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::getRequiredChildren(std::vector<const FactorizedTableNode*>& todo) const {
   for (const FactorizedTableNode& child : children | views::filter([](auto& child) { return child.is_required(); })) {
      todo.push_back(&child);
   }
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorInsertFunction(FileWriter& out, const FactorizedTableNode& other) const {
   if (is_root()) {
      // find child at which something is inserted
      const FactorizedTableNode* lastChild = &other;
      while (!lastChild->children.empty())
         lastChild = &lastChild->children.back();

      // generate function header from the last child
      out << "void insert(" << other.class_name << "& other, " << (views::enumerate(lastChild->ius) | views::transform([](auto&& coIu) { return fw::lc(std::get<1>(coIu)->type, " val", std::get<0>(coIu)); }))
          << ") {" << fw::endl();
   }

   std::vector<const FactorizedTableNode*> todo = {this};
   const FactorizedTableNode* curOther = nullptr;
   size_t offsetIus, offsetChildren;

   size_t idx = 0;
   while (!todo.empty()) {
      auto lcRefPrev = fw::lc("lb", iter_idx, "i", idx);
      auto lcRef = fw::lc("lb", iter_idx, "i", ++idx);
      // fw::get(curOther->children.size(), fw::lc(lcRefPrev, ".back()"));
      auto childHandle = curOther ? fw::lc("std::get<", curOther->children.size() + curOther->ius.size() - 1, ">(", lcRefPrev, ".back())") : fw::lc("other.cl");
      curOther = curOther ? &curOther->children.back() : &other;

      out.setSeparator(" || ", false, false);
      offsetIus = 0, offsetChildren = curOther->ius.size();
      out << curOther->class_name << "::" << curOther->list_type << "& " << lcRef << " = " << childHandle << fw::sendl()
          << "if (" << lcRef << ".empty()";
      for (const FactorizedTableNode* child : todo) {
         child->genIteratorInsertFunAssignmentHelper(out, offsetIus, offsetChildren, lcRef, "!=", "!=");
      }
      out << ") {" << fw::endl()
          << lcRef << ".emplace_back();" << fw::endl();

      std::vector<const FactorizedTableNode*> nextTodo;
      offsetIus = 0, offsetChildren = curOther->ius.size();
      out.setSeparator(";", true);
      for (const FactorizedTableNode* child : todo) {
         child->genIteratorInsertFunAssignmentHelper(out, offsetIus, offsetChildren, lcRef, "=", "<<");
         child->getRequiredChildren(nextTodo);
      }
      out << ";" << fw::endl()
          << "}" << fw::endl();
      todo = std::move(nextTodo);
   }

   auto lcRefPrev = fw::lc("lb", iter_idx, "i", idx);
   auto lcRef = fw::lc("lb", iter_idx, "i", ++idx);
   auto childHandle = curOther ? fw::lc("std::get<", curOther->children.size() + curOther->ius.size() - 1, ">(", lcRefPrev, ".back())") : fw::lc("other.cl");
   curOther = curOther ? &curOther->children.back() : &other;
   out << curOther->class_name << "::" << curOther->list_type << "& " << lcRef << " = " << childHandle << fw::sendl()
       << lcRef << ".emplace_back(" << (views::enumerate(curOther->ius) | views::transform([](auto&& coIu) { return fw::lc("val", std::get<0>(coIu)); })) << ");" << fw::endl();

   if (is_root()) out << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTableNode::genIteratorDereference(FileWriter& out) const {
   if (get_subiterator().empty()) {
      out << "// operator* not generated since no order was set." << fw::endl();
      return;
   }
   out << "reference operator*() {" << fw::endl();
   genVariables(out);
   out.setSeparator(", ");
   out << "return std::tie(" << get_subiterator() << ");" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::translator
// ---------------------------------------------------------------------------------------------------
