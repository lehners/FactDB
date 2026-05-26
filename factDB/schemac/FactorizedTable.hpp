#ifndef H_FACTDB_FACTDB_SCHEMAC_FACTORIZEDTABLE_HPP
#define H_FACTDB_FACTDB_SCHEMAC_FACTORIZEDTABLE_HPP

#include "factDB/infra/Types.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>

namespace factDB::schemac {

// TODO add Iterator for subset of columns and use multiplicity from other columns
//  (!do not enumerate tuples which × ∅, other tuples **at least** once!)
// TODO add Iterator for subset of columns and ignore multiplicity from other columns

class FactorizedTable {
   using KeyT = Integer;
   using ValueT = Integer;

   std::size_t sizeVar = 0;
   std::size_t columns = 1;

   class FactTuple {
      friend class FactorizedTable;
      KeyT factor = Integer(0);
      std::vector<std::vector<ValueT>> values;

      FactTuple() = default;

      public:
      explicit FactTuple(KeyT key, size_t cols=1) : factor(key) { for (size_t i = 0; i < cols; i++) values.emplace_back(); };
      FactTuple(const FactTuple&) = delete;
      FactTuple operator=(const FactTuple&) = delete;

      private:
      FactTuple& append(ValueT val, size_t index) {
         assert(index < values.size());
         values[index].push_back(val);
         return *this;
      }
      [[nodiscard]] std::size_t size(size_t idx) const {
         assert(idx < values.size());
         return values[idx].size();
      }
      [[nodiscard]] std::size_t empty(size_t idx) const {
         assert(idx < values.size());
         return values[idx].empty();
      }
      [[nodiscard]] std::size_t emptyChilds() {
         for (const auto& c : values)
            if (c.empty())
               return true;
         return false;
      }
      [[nodiscard]] const ValueT* data(size_t idx) const {
         assert(idx < values.size());
         return values[idx].data();
      }
      [[nodiscard]] const ValueT* endPtr(size_t idx) const {
         assert(idx < values.size());
         return values[idx].data() + values[idx].size();
      }
      [[nodiscard]] size_t getSizeExcludingCol(size_t col) const {
         assert(col < values.size());
         size_t total = 1;
         for (size_t i = 0; i < values.size(); i++) {
            if (i == col) continue;
            total *= values[i].size();
         }
         return total;
      }
   };

   struct FactIterator {
      friend class FactorizedTable;

      using iterator_category = std::forward_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = std::tuple<int, const std::vector<Integer>>;
      using pointer = std::tuple<const KeyT*, const std::vector<const Integer*>>; // or also valueType*
      using reference = std::tuple<const KeyT&, const std::vector<const Integer*>&>;

      private:
      const std::unique_ptr<FactTuple>* tuple;
      const std::unique_ptr<FactTuple>* end;
      std::vector<const ValueT*> curValues;
      std::vector<size_t> idxs;

      FactIterator(const FactorizedTable& table, const std::vector<size_t>& idxsP)
         : tuple(table.tuples.data()), end(table.tuples.data() + table.tuples.size()), curValues(idxsP.size()-1, nullptr), idxs(idxsP) {
         std::sort(idxs.begin(), idxs.end());
         while (tuple != end) {
            if (tuple->get()->emptyChilds()) {
               ++tuple;
               continue;
            }
            for (size_t i = 1; i < idxs.size(); i++) {
               curValues[i-1] = tuple->get()->data(idxs[i]-1);
            }
            break;
         }
         if (tuple == end) {
            tuple = nullptr;
            end = nullptr;
         }
      };

      public:
      FactIterator() : tuple(nullptr), curValues(){};

      reference operator*() const { return std::tie(tuple->get()->factor, curValues); }
      pointer operator->() { return std::make_tuple(&tuple->get()->factor, curValues); }

      // Prefix increment
      FactIterator& operator++() {
         if (++curValues.back() == tuple->get()->endPtr(idxs.back() - 1)) {
            for (size_t i = 1; i < curValues.size(); i++) {
               const auto idx = curValues.size() - 1 - i;
               if (++curValues[idx] != tuple->get()->endPtr(idxs[idx + 1] - 1)) {
                  for (size_t i2 = idx + 1; i2 < curValues.size(); i2++) {
                     curValues[i2] = tuple->get()->data(idxs[i2+1]-1);
                  }
                  return *this;
               }
            }
            while (++tuple != end) {
               if (tuple->get()->emptyChilds())
                  continue;
               for (size_t i = 1; i < idxs.size(); i++) {
                  curValues[i-1] = tuple->get()->data(idxs[i]-1);
               }
               break;
            }
            if (tuple == end) {
               curValues.clear();
               tuple = nullptr;
            }
         }
         return *this;
      }

      // Postfix increment
      FactIterator operator++(int) {
         FactIterator tmp = *this;
         ++(*this);
         return tmp;
      }

      bool operator==(const FactIterator& other) const { return tuple == other.tuple && curValues == other.curValues; };
      bool operator!=(const FactIterator& other) const { return tuple != other.tuple || curValues != other.curValues; };
   };

   std::vector<std::unique_ptr<FactTuple>> tuples;

   public:
   FactorizedTable() = default;
   FactorizedTable(FactorizedTable&) = delete;
   FactorizedTable operator=(FactorizedTable&) = delete;

   FactTuple& insert(const ValueT key) {
      return *tuples.emplace_back(std::make_unique<FactTuple>(key)).get();
   }

   FactTuple& insert(const int key, const int val, const size_t col=0) {
      return insert(Integer(key), Integer(val), col);
   }

   FactTuple& insert(FactTuple* handle, const int val, const size_t col=0) {
      return insert(handle, Integer(val), col);
   }

   FactTuple& insert(const ValueT& key, const KeyT& val, const size_t col=0) { // NOLINT
      if (tuples.empty() || tuples.back().get()->factor != key) {
         tuples.emplace_back(std::make_unique<FactTuple>(key, columns));
      }
      sizeVar += tuples.back().get()->getSizeExcludingCol(col);
      return tuples.back().get()->append(val, col);
   }

   FactTuple& insert(FactTuple* handle, const KeyT val, const size_t col=0) {
      assert(handle != nullptr);

      sizeVar += handle->getSizeExcludingCol(col);
      return handle->append(val, col);
   }

   [[nodiscard]] FactTuple* findKey(const ValueT key) const{
      for (const auto& t: tuples) {
         if (t.get()->factor == key) {
            return t.get();
         }
      }
      return nullptr;
   }

   void appendCol() {
      for(auto& c : tuples) {
         c->values.emplace_back();
      }
      ++columns;
      sizeVar = 0;
   }

   struct IterStruct {
      std::vector<size_t> vec;
      const FactorizedTable* ptr;
      [[nodiscard]] FactIterator begin() const { return {*ptr, vec}; }
      [[nodiscard]] FactIterator end() const { return {}; }
   };

   [[nodiscard]] IterStruct iter(std::vector<size_t> vec) const {
      return {std::move(vec), this};
   }

   [[nodiscard]] FactIterator begin() const { return {*this, {0,1}}; }
   [[nodiscard]] FactIterator end() const { return {}; }
   [[nodiscard]] std::size_t size() const { return sizeVar; }

   void print() {
      for (const auto& i : tuples) {
         std::cout << i.get()->factor;
         bool first = true;
         for (const auto& j : i.get()->values) {
            std::cout << (first ? " × {" : "} × {");
            bool firstInner = true;
            for (const auto& vj : j) {
               std::cout << (firstInner ? "" : ", ") << vj;
               firstInner = false;
            }
            first = false;
         }
         std::cout << "}" << std::endl;
      }
   }
};

} // namespace factDB::schemac

#endif // H_FACTDB_FACTDB_SCHEMAC_FACTORIZEDTABLE_HPP
