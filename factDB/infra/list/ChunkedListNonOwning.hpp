#pragma once
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class OwningChunkedList;
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningChunkedList : public ChunkedListBase<ElementTypeParam> {
   public:
   using ElementType = ElementTypeParam;
   using ParentClass = ChunkedListBase<ElementType>;
   using OwningVariant = OwningChunkedList<ElementType>;

   using Iterator = ParentClass::Iterator;
   using ConstIterator = ParentClass::ConstIterator;

   private:
   size_t sizeVal = 0;

   void merge(const std::vector<typename ParentClass::ChunkInfo>& otherChunks, size_t otherSize);

   public:
   NonOwningChunkedList() = default;

   void merge(const OwningChunkedList<ElementType>& otherList) { merge(otherList.chunks, otherList.size()); }
   void merge(const NonOwningChunkedList<ElementType>& otherList) { merge(otherList.chunks, otherList.size()); }

   [[nodiscard]] size_t size() const { return sizeVal; }

   [[nodiscard]] bool operator==(const OwningVariant& other) const { return static_cast<const ParentClass&>(*this) == static_cast<const ParentClass&>(other); }
   [[nodiscard]] bool operator==(const NonOwningChunkedList& other) const = default;
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
void NonOwningChunkedList<T>::merge(const std::vector<typename ParentClass::ChunkInfo>& otherChunks, size_t otherSize) {
   this->chunks.reserve(this->chunks.size() + otherChunks.size());
   for (auto& c : otherChunks) {
      this->chunks.emplace_back(c);
   }
   sizeVal += otherSize;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------
