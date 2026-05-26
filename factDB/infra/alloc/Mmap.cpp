#include "factDB/infra/alloc/Mmap.hpp"
#include <cstdio>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
//---------------------------------------------------------------------------
namespace factDB {
//---------------------------------------------------------------------------
void Mmap::prefault(void* data, size_t size) {
   char* mem = static_cast<char*>(data);
   for (std::size_t i = 0; i < size; i += 4096)
      mem[i] = 0;
}
//---------------------------------------------------------------------------
Mmap Mmap::mapMemory(size_t size) {
   Mmap result;
   result.size_ = size;
#if defined(NDEBUG) and not defined(__APPLE__)
   result.data_ = static_cast<char*>(mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
   if (size >= 2'000'000)
      madvise(result.data_, size, MADV_HUGEPAGE);
#else
   result.data_ = static_cast<char*>(aligned_alloc(4096, size));
#endif
   return result;
}
//---------------------------------------------------------------------------
void Mmap::reset() noexcept {
   if (data_) {
#ifdef NDEBUG
      munmap(data_, size_);
#else
      if (file != -1)
         munmap(data_, size_);
      else
         free(data_);
#endif
      data_ = nullptr;
      size_ = 0;
   }
   if (file != -1) {
      close(file);
      file = -1;
   }
}
//---------------------------------------------------------------------------
} // namespace factDB
