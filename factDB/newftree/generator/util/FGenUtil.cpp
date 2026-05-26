#include "factDB/newftree/generator/util/FGenUtil.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
static size_t handleID = 0;
// ---------------------------------------------------------------------------------------------------
FWContainer push_back_helper(FTreeTranslator& translator, const FNode* owner, const FHandle& parentHandle, FHandle& childHandle, const IU& pivot, FWContainer&& iterator, const FWContainer& tlsLocal) {
   childHandle.setUniqueID();

   auto resultContainer = childHandle.isEmpty() ? fw::nop() : fw::fmt("auto {} = ", childHandle.gen());
   [[maybe_unused]] auto useTLS = SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get();

   if (owner == &translator.tree_.getRootNode()) {
      if (tlsLocal.isNOP()) {
         assert(!useTLS);
         return fw::fmt("{}{}.push_back({}, {});", std::move(resultContainer), translator.name(), translator.iuEnum(pivot), std::move(iterator));
      } else {
         assert(useTLS);
         return fw::fmt("{}{}.push_back({}, {}, {});", std::move(resultContainer), translator.name(), translator.iuEnum(pivot), std::move(iterator), tlsLocal);
      }
   } else {
      assert(tlsLocal.isNOP());
      return fw::fmt("{}{}.push_back(*{}, {}, {});", std::move(resultContainer), translator.name(), parentHandle.gen(), translator.iuEnum(pivot), std::move(iterator));
   }
}
// ---------------------------------------------------------------------------------------------------
} // anonymous namespace
// ---------------------------------------------------------------------------------------------------
void FHandle::setUniqueID() {
   id = handleID++;
}
// ---------------------------------------------------------------------------------------------------
void FHandle::resetCounters() {
   handleID = 0;
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeUtil::genParamList() {
   return fw::iter(node.getIUs() | std::views::transform([](const IU* iu) {
                      return fw::fmt("const {}& {}", iu->type.toString(), iu);
                   }));
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeUtil::genTypes(bool references) {
   return fw::iter(node.getIUs() | std::views::transform([references](const IU* iu) {
                      return fw::lc(iu->type.toString(), fw::condition(references, "&"));
                   }));
}
// ---------------------------------------------------------------------------------------------------
size_t FTreeUtil::getSizeIdx() {
   assert(SettingBase::getSetting<bool>("codegen.factorized.sizePerList")->get());
   return 0;
}
// ---------------------------------------------------------------------------------------------------
size_t FTreeUtil::getIUIdx(const factDB::IU& iu) const {
   auto it = std::find(node.getIUs().begin(), node.getIUs().end(), &iu);
   assert(it != node.getIUs().end());

   size_t ret = std::distance(node.getIUs().begin(), it);
   ret += SettingBase::getSetting<bool>("codegen.factorized.sizePerList")->get();
   ret += SettingBase::getSetting<bool>("codegen.factorized.iusFirst")->get() ? 0 : node.getChildren().size();
   return ret;
}
// ---------------------------------------------------------------------------------------------------
size_t FTreeUtil::getChildIdx(const factDB::FNode& child) const {
   auto it = std::find_if(node.getChildren().begin(), node.getChildren().end(), [&](const auto& cur) { return cur.get() == &child; });
   assert(it != node.getChildren().end());

   auto fSizePerList = SettingBase::getSetting<bool>("codegen.factorized.sizePerList")->get();
   auto iuOffset = SettingBase::getSetting<bool>("codegen.factorized.iusFirst")->get() ? node.getIUs().size() : 0;

   return std::distance(node.getChildren().begin(), it) + fSizePerList + iuOffset;
}
// ---------------------------------------------------------------------------------------------------
size_t FTreeUtil::getChildIdx(size_t childIdx) const {
   auto fSizePerList = SettingBase::getSetting<bool>("codegen.factorized.sizePerList")->get();
   auto iuOffset = SettingBase::getSetting<bool>("codegen.factorized.iusFirst")->get() ? node.getIUs().size() : 0;
   return childIdx + fSizePerList + iuOffset;
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeUtil::getQualifiedIteratorName(const FIterator& iterator) {
   return fw::fmt("{}::FactorizedIterator{}", FTreeTranslator(iterator.getOwner()).classname(), iterator.getIteratorID());
}
// ---------------------------------------------------------------------------------------------------
const FNode* FTreeTranslator::getIUOwner(const factDB::IU& pivot) {
   std::function<const FNode*(const FNode&)> find_key = [&](const FNode& node) -> const FNode* {
      if (node.containsIU(pivot)) {
         return &node;
      } else {
         for (auto& child : node.getChildren()) {
            if (auto res = find_key(*child); res != nullptr)
               return res;
         }
      }
      return nullptr;
   };

   return find_key(tree_.getRootNode());
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeTranslator::push_back(const FHandle& parentHandle, FHandle& childHandle, const OrderedIUSet& ius, const FWContainer& tlsLocal) {
   assert(!ius.empty());
   auto& pivot = *ius.front();
   const FNode* owner = getIUOwner(pivot);

   assert(ius.size() == owner->getIUs().size());
   return push_back_helper(*this, owner, parentHandle, childHandle, pivot, fw::iter(owner->getIUs()), tlsLocal);
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeTranslator::push_back(const FHandle& parentHandle, FHandle& childHandle, const IU& pivot, std::initializer_list<FWContainer>&& values, const FWContainer& tlsLocal) {
   const FNode* owner = getIUOwner(pivot);

   FWContainer printValues = fw::func([values, owner](FileWriter& o) {
      o << fw::pushSeparator(", ");
      for (auto iuIter = owner->getIUs().begin(); auto& val : values) {
         o << fw::separator() << fw::fmt("{}({})", (*iuIter)->type, val);
         ++iuIter;
      }
      o << fw::popSeparator();
   });

   assert(values.size() == owner->getIUs().size());
   return push_back_helper(*this, owner, parentHandle, childHandle, pivot, std::move(printValues), tlsLocal);
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeTranslator::getInsertHandleType() const {
   // the new subtree is inserted at a child of the chain of merged nodes
   // the root of the new subtree is never a merge node.
   const FNode* insertionPos = &tree_.getRootNode();
   while (insertionPos->getMergedNode()) {
      insertionPos = insertionPos->getMergedNode();
   }
   // the insertion pos may have multiple children, but the last one corresponds to the inserted subtree.
   return fw::lc(classname(), "::", FTreeUtil(*insertionPos).iteratorType());
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeTranslator::reserve(const FHandle& handle, const IU& pivot, const FWContainer& size) {
   assert(!tree_.getRootNode().containsIU(pivot));
   return fw::fmt("{}.reserve({}, {}, {})", name_, handle.gen(), iuEnum(pivot), size);
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeTranslator::appendEmptyTree(const FHandle& handle, const FWContainer& tlsLocal, bool useHandleStorage) {
   assert(getFTree().getTopIterator() != nullptr);
   auto topIter = FIteratorTranslator::getDefaultName(*getFTree().getTopIterator());
   assert(SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get() != tlsLocal.isNOP());
   if (tlsLocal.isNOP() && useHandleStorage) {
      return fw::fmt("auto {} = {}.appendEmptyTree({}, {});", handle.gen(), name_, std::move(topIter), handleStorage());
   } else if (useHandleStorage) {
      assert(!tlsLocal.isNOP());
      return fw::fmt("auto {} = {}.appendEmptyTree({}, {}, {});", handle.gen(), name_, std::move(topIter), tlsLocal, handleStorage());
   } else if (tlsLocal.isNOP()) {
      assert(!useHandleStorage);
      return fw::fmt("auto [{}, {}] = {}.appendEmptyTree({});", handle.gen(), handleStorage(), name_, std::move(topIter));
   } else {
      return fw::fmt("auto [{}, {}] = {}.appendEmptyTree({}, {});", handle.gen(), handleStorage(), name_, std::move(topIter), tlsLocal);
   }
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeTranslator::insertAsChild(const FHandle& handle, bool useInsertHandleStorage) {
   assert(getFTree().getLowerIterator() != nullptr);
   auto toInsertIterator = FIteratorTranslator::getDefaultName(*getFTree().getLowerIterator());
   if (useInsertHandleStorage)
      return fw::fmt("{}.insertAsChild({}, {}, {});", name_, handle.gen(), std::move(toInsertIterator), insertHandleStorage());
   else
      return fw::fmt("{}.insertAsChild({}, {});", name_, handle.gen(), std::move(toInsertIterator));
}
// ---------------------------------------------------------------------------------------------------
FWContainer FIteratorTranslator::get(const IU& iu) {
   assert(iterator.getIteratedIUs().contains(iu));
   return fw::fmt("{}.get<{}::{}>()", name(), FTreeTranslator(iterator.getOwner()).classname(), iu);
}
// ---------------------------------------------------------------------------------------------------
FWContainer FTreeTranslator::constructMergingIterator() const {
   assert(getFTree().isComposedTree());
   auto mergingIterator = getMergingIteratorTranslator();
   auto mergingIteratorName = FIteratorTranslator::getDefaultName(*tree_.getMergingIterator()); // cannot use translator since it is destructed
   auto upperIteratorName = FIteratorTranslator::getDefaultName(*tree_.getTopIterator());
   auto lowerIteratorName = FIteratorTranslator::getDefaultName(*tree_.getLowerIterator());
   return fw::fmt("{} {} {{ {}, {} }};", mergingIterator.getQualifiedIteratorName(), std::move(mergingIteratorName), std::move(upperIteratorName), std::move(lowerIteratorName));
}
// ---------------------------------------------------------------------------------------------------
FIteratorTranslator FTreeTranslator::getIteratorTranslator(const FIterator& iter, FWContainer instanceName) const {
   if (instanceName.isNOP())
      instanceName = FIteratorTranslator::getDefaultName(iter);
   return FIteratorTranslator(*this, iter, std::move(instanceName));
}
// ---------------------------------------------------------------------------------------------------
FIteratorTranslator FTreeTranslator::getFrontIteratorTranslator(FWContainer instanceName) const {
   return getIteratorTranslator(tree_.getIterators().front(), std::move(instanceName));
}
// ---------------------------------------------------------------------------------------------------
FIteratorTranslator FTreeTranslator::getMergingIteratorTranslator(FWContainer instanceName) const {
   assert(getFTree().getMergingIterator() != nullptr);
   return getIteratorTranslator(*tree_.getMergingIterator(), std::move(instanceName));
}
// ---------------------------------------------------------------------------------------------------
FIteratorTranslator FTreeTranslator::getTopIteratorTranslator(FWContainer instanceName) const {
   assert(getFTree().getTopIterator() != nullptr);
   return getIteratorTranslator(*getFTree().getTopIterator(), std::move(instanceName));
}
// ---------------------------------------------------------------------------------------------------
FIteratorTranslator FTreeTranslator::getLowerIteratorTranslator(FWContainer instanceName) const {
   assert(getFTree().getLowerIterator() != nullptr);
   return getIteratorTranslator(*getFTree().getLowerIterator(), std::move(instanceName));
}
// ---------------------------------------------------------------------------------------------------
bool FTreeTranslator::hasComposedSubiterator() const {
   auto available = tree_.collectComposedAvailableNodes();
   auto required = tree_.getMergingIterator()->getRequiredNodeIDs();
   for (auto& id : required) {
      if (!available.contains(id))
         return true;
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
FIteratorTranslator FIteratorTranslator::getReferencedIteratorTranslator(FWContainer name) const {
   assert(iterator.hasReferencedIterator());
   if (name.isNOP())
      name = getDefaultName(iterator.getReferencedIterator());
   FIteratorTranslator translator(iterator.getReferencedIterator(), std::move(name));
   translator.ftreeName = fw::lc(ftreeName);
   return translator;
}
// ---------------------------------------------------------------------------------------------------
