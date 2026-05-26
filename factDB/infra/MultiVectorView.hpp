#pragma once

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <vector>

/**
 * MultiVectorView<T>
 *
 * A mutable view over a collection of contiguous ranges (e.g. std::vector
 * with any allocator, std::array, raw arrays, …).  Only the data pointer
 * and size of each range are stored – the view is completely independent
 * of the original container type.
 *
 * Elements are mutable: dereferencing the iterator yields T&.
 *
 * Interface
 * ─────────
 *   insert(data, size)          – register a raw span
 *   insert(contiguous_range)    – convenience overload for anything with
 *                                  .data() / .size()  (vector, array, …)
 *   begin() / end()             – iterate over all elements
 *   iterator_at(pos)            – iterator starting at flat index pos
 *   size() / empty()            – total element count
 *   vector_count()              – number of spans registered
 */
template <typename T>
class MultiVectorView {
   // ----------------------------------------------------------------
   // Internal span (pointer + length, no ownership)
   // ----------------------------------------------------------------
   struct Span {
      T* data;
      std::size_t size;
   };

   public:
   // ----------------------------------------------------------------
   // Iterator
   // ----------------------------------------------------------------
   class iterator {
      public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using pointer = T*;
      using reference = T&;

      iterator() = default;

      iterator(const std::vector<Span>* spans,
               std::size_t span_idx,
               std::size_t elem_idx)
         : spans_(spans), span_idx_(span_idx), elem_idx_(elem_idx) {
         skip_empty();
      }

      reference operator*() const { return (*spans_)[span_idx_].data[elem_idx_]; }
      pointer operator->() const { return &(**this); }

      iterator& operator++() {
         ++elem_idx_;
         if (elem_idx_ >= (*spans_)[span_idx_].size) {
            ++span_idx_;
            elem_idx_ = 0;
            skip_empty();
         }
         return *this;
      }

      iterator operator++(int) {
         iterator tmp = *this;
         ++(*this);
         return tmp;
      }

      bool operator==(const iterator& o) const {
         return span_idx_ == o.span_idx_ && elem_idx_ == o.elem_idx_;
      }
      bool operator!=(const iterator& o) const { return !(*this == o); }

      private:
      const std::vector<Span>* spans_ = nullptr;
      std::size_t span_idx_ = 0;
      std::size_t elem_idx_ = 0;

      void skip_empty() {
         while (span_idx_ < spans_->size() &&
                (*spans_)[span_idx_].size == 0)
            ++span_idx_;
      }
   };

   // ----------------------------------------------------------------
   // Insert
   // ----------------------------------------------------------------

   /** Register a raw span.  The memory must outlive this view. */
   void insert(T* data, std::size_t size) {
      spans_.push_back({data, size});
   }

   /**
     * Convenience overload for any contiguous range that exposes
     * .data() and .size() — std::vector<T, Alloc>, std::array<T,N>, etc.
     */
   template <typename Range>
   void insert(Range& range) {
      insert(range.data(), range.size());
   }

   // ----------------------------------------------------------------
   // Iteration
   // ----------------------------------------------------------------

   iterator begin() const { return iterator(&spans_, 0, 0); }
   iterator end() const { return iterator(&spans_, spans_.size(), 0); }

   /**
     * Iterator starting at global flat index pos.
     * pos == size() is valid and returns end().
     * Throws std::out_of_range for pos > size().
     */
   iterator iterator_at(std::size_t pos) const {
      for (std::size_t si = 0; si < spans_.size(); ++si) {
         if (pos < spans_[si].size)
            return iterator(&spans_, si, pos);
         pos -= spans_[si].size;
      }
      if (pos == 0)
         return end();
      throw std::out_of_range("MultiVectorView::iterator_at: position out of range");
   }

   // ----------------------------------------------------------------
   // Utilities
   // ----------------------------------------------------------------

   std::size_t size() const {
      std::size_t n = 0;
      for (const auto& s : spans_) n += s.size;
      return n;
   }

   bool empty() const { return size() == 0; }
   std::size_t vector_count() const { return spans_.size(); }

   private:
   std::vector<Span> spans_;
};