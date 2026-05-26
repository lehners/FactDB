// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/ListTypeEnum.hpp"
#include "factDB/infra/Config.hpp"
#include <string>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
const std::string curNamespace = "factDB::infra::list::";
// ---------------------------------------------------------------------------------------------------
std::string_view toString(ListTypeEnum lte) {
   switch (lte) {
      case ChunkedList: return "ChunkedList";
      case LinkedList: return "LinkedList";
      case TbbConcurrentVector: return "TbbConcurrentVector";
      case InlineConcurrentVector: return "InlineConcurrentVector";
      case TLSDequeList: return "TLSDeque";
      case TLFastDequeList: return "TLFastDeque";
      case TbbTLFastDequeList: return "TbbTLFastDeque";
      case FastDequeEnum: return "FastDeque";
      case FastDequeEnumInlining: return "FastDequeInlined";
      case STLVector: return "StlVector";
      case STLDeque: return "StlDeque";
      case Auto: factDB::unreachable();
   }
   factDB::unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::string toClassName(ListTypeEnum lte, bool owning) {
   std::string owningString = owning ? "Owning" : "NonOwning";
   switch (lte) {
      case TLSDequeList:
      case TLFastDequeList:
      case TbbTLFastDequeList:
         return curNamespace + std::string(toString(lte));
      case FastDequeEnum:
      case FastDequeEnumInlining:
         return curNamespace + (owning ? "" : "NonOwning") + std::string(toString(lte));
      case ChunkedList:
      case TbbConcurrentVector:
      case InlineConcurrentVector:
      case Auto:
      case LinkedList:
      case STLVector:
      case STLDeque:
         return curNamespace + owningString + std::string(toString(lte));
   }
   factDB::unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool hasLock(ListTypeEnum val) {
   switch (val) {
      case ChunkedList:
      case TbbConcurrentVector:
      case InlineConcurrentVector:
      case TLSDequeList:
      case TLFastDequeList:
      case TbbTLFastDequeList:
      case Auto:
      case LinkedList:
         return false;
      case STLVector:
      case STLDeque:
      case FastDequeEnum:
      case FastDequeEnumInlining:
         return true;
   }
   factDB::unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool hasReversedIterator(ListTypeEnum val) {
   switch (val) {
      case ChunkedList:
      case TbbConcurrentVector:
      case InlineConcurrentVector:
      case TLSDequeList:
      case TLFastDequeList:
      case TbbTLFastDequeList:
      case STLVector:
      case STLDeque:
      case Auto:
      case FastDequeEnum:
      case FastDequeEnumInlining:
         return false;
      case LinkedList:
         return true;
   }
   factDB::unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool shouldCompactLastChunk(ListTypeEnum val) {
   switch (val) {
      case ChunkedList:
         return true;
      case TbbConcurrentVector:
      case InlineConcurrentVector:
      case TLSDequeList:
      case TLFastDequeList:
      case TbbTLFastDequeList:
      case Auto:
      case LinkedList:
      case STLVector:
      case STLDeque:
      case FastDequeEnum:
      case FastDequeEnumInlining:
         return false;
   }
   factDB::unreachable();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------