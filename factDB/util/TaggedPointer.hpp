// ---------------------------------------------------------------------------------------------------
#include <cstdint>
// ---------------------------------------------------------------------------------------------------
namespace factDB::util {
// ---------------------------------------------------------------------------------------------------
class TaggedPointer {
   public:
#ifdef __x86_64__
   /// We can use pointer tagging on 64bit Intel machines
   static constexpr bool useTagging = true;
   /// We use 16 bits for tags
   static constexpr unsigned taggingBits = 16;
   /// The mask to retrieve the tag
   static constexpr uint64_t taggingMask = (1ULL << taggingBits) - 1;
#else
   /// In general we cannot use tagging
   static constexpr bool useTagging = false;
   /// No bits available
   static constexpr unsigned taggingBits = 0;
   /// No tag available
   static constexpr uintptr_t taggingMask = 0;
#endif

   protected:
   uintptr_t ptr = 0;

   TaggedPointer() = default;

   public:
   TaggedPointer(const void* ptr_, uint16_t tag = 0) : ptr(useTagging ? ((reinterpret_cast<uintptr_t>(ptr_) << taggingBits) + tag) : reinterpret_cast<uintptr_t>(ptr_)) {}

   void updateTag(uint16_t tag) {
      if constexpr (useTagging) {
         ptr = (ptr & ~taggingMask) + tag;
      }
   }

   void* getPointer() const {
      if constexpr (useTagging) {
         return reinterpret_cast<void*>(ptr >> taggingBits);
      } else {
         return reinterpret_cast<void*>(ptr);
      }
   }

   uint16_t getTag() const {
      if constexpr (useTagging) {
         return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ptr & taggingMask));
      } else {
         return 0;
      }
   }

   uintptr_t getRaw() const { return ptr; }
   void setRawPointer(void* ptr_) { ptr = reinterpret_cast<uintptr_t>(ptr_); }
};
// ---------------------------------------------------------------------------------------------------
template <class T>
struct TypedTaggedPointer {
   TaggedPointer ptr;

   explicit TypedTaggedPointer(const T* ptr_, uint16_t tag = 0) : ptr(ptr_, tag){};
   void updateTag(uint16_t tag) { ptr.updateTag(tag); }
   const T* getPointer() const { return static_cast<const T*>(ptr.getPointer()); }
   uint16_t getTag() const { return ptr.getTag(); }
   uintptr_t getRaw() const { return ptr.getRaw(); }

   static TypedTaggedPointer<T> fromRawPtr(T* ptr_) {
      TypedTaggedPointer<T> ptr(nullptr);
      ptr.ptr.setRawPointer(ptr_);
      return ptr;
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::util
// ---------------------------------------------------------------------------------------------------