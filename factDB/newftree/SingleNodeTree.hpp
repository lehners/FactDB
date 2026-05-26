#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/ListBase.hpp" // CodegenFactorized.cpp(97:7)
#include "factDB/queryc/NewFileWriter.hpp" // CodegenExpanded.cpp(37:4)
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <typename SingleNodeTree, size_t... ElementIdx>
struct Iterator;
// ---------------------------------------------------------------------------------------------------
template <typename T>
struct IteratorWrapper {
   using Iterator = T;
};
// ---------------------------------------------------------------------------------------------------
template <typename... ElementTs>
struct SingleNodeTree {
   /*
   digraph structs {
      node [shape=record];
      _node0 [label="{{Node 0 }|{ elem... }}"];
   }
   */

   static constexpr size_t ElementSize = sizeof...(ElementTs);
   using ElementTuple = std::tuple<ElementTs...>;
   using ListType = IteratorWrapper<ElementTuple>;

   ElementTuple elem;

   size_t size() const { return 1; }
   void print(FileWriter& out) const { out << fw::separator() << "(" << std::get<0>(elem).value << ", " << std::get<1>(elem).value << ")" << fw::endl(); }

   static SingleNodeTree* reinterpretTuple(ElementTuple& elem) { return reinterpret_cast<SingleNodeTree*>(&elem); }

   template <typename T>
   static const ElementTuple& asTuple(const T& elem) {
      static_assert(sizeof(T) == sizeof(ElementTuple));
      return reinterpret_cast<const ElementTuple&>(elem);
   }

   using DefaultIterator = typename decltype([]<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::type_identity<Iterator<SingleNodeTree, Is...>>{};
   }(std::make_index_sequence<sizeof...(ElementTs)>{}))::type;
};
// --------------------------------------------------------------------------------------------------
template <typename SingleNodeTree, size_t... ElementIdx>
struct Iterator {
   static_assert(((ElementIdx < SingleNodeTree::ElementSize) && ...), "Tuple index out of bounds");

   using reference = const std::tuple<std::tuple_element_t<ElementIdx, typename SingleNodeTree::ElementTuple>...>;

   const SingleNodeTree::ElementTuple* elements;

   Iterator(SingleNodeTree& fti) : elements(&fti.elem) {}
   Iterator(const SingleNodeTree::ElementTuple& elem) : elements(&elem) {}

   Iterator(Iterator&& other) noexcept = default;
   Iterator(const Iterator& other) noexcept = default;

   Iterator& operator=(const Iterator& other) noexcept = default;
   Iterator& operator=(Iterator&& other) noexcept {
      elements = std::move(other.elements);
      return *this;
   }

   Iterator& operator++() {
      elements = nullptr;
      assert(isLast());
      return *this;
   }

   static Iterator begin(SingleNodeTree& t) { return Iterator(t); }

   bool isLast() const { return elements == nullptr; }
   bool isAnyLast() const { return elements != nullptr; }
   bool isSubtreeEmpty() const { return false; }
   size_t iteratedTupleSize([[maybe_unused]] bool useCache = true) const { return 1; }

   static std::pair<size_t, std::vector<size_t>> iterableSize(SingleNodeTree&) { return {1, {}}; }

   bool operator==([[maybe_unused]] const Iterator& other) const { return *elements == *other.elements; }
   bool operator!=([[maybe_unused]] const Iterator& other) const { return *elements != *other.elements; }

   // Iterator& operator++() { not_implemented(); }

   reference operator*() const { return std::tie(std::get<ElementIdx>(*elements)...); }
};
// ---------------------------------------------------------------------------------------------------
template <typename SingleNodeTree>
struct Iterator<SingleNodeTree> { // special case for the empty iterator
   using reference = std::tuple<>;

   const SingleNodeTree::ElementTuple* elements; // keep because may be required to create reference iterators

   Iterator(SingleNodeTree& fti) : elements(&fti.elem) {}
   Iterator(const SingleNodeTree::ElementTuple& elem) : elements(&elem) {}

   static Iterator begin(SingleNodeTree& t) { return Iterator(t); }

   // template <size_t... T>
   // static Iterator fromReferencedIterator(const Iterator<T...>& r) { return {r.elements}; }

   bool isLast() const { return true; }
   bool isAnyLast() const { return false; }
   bool isSubtreeEmpty() const { return false; }
   size_t iteratedTupleSize([[maybe_unused]] bool useCache = true) const { return 1; }

   static std::pair<size_t, std::vector<size_t>> iterableSize(SingleNodeTree&) { return {1, {}}; }

   bool operator==([[maybe_unused]] const Iterator& other) const { return true; }
   bool operator!=([[maybe_unused]] const Iterator& other) const { return false; }

   // Iterator& operator++() { not_implemented(); }

   reference operator*() const { return std::tie(); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
