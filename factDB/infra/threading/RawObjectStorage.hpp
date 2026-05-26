#pragma once
//---------------------------------------------------------------------------
#include <cstddef>
#include <new>
//---------------------------------------------------------------------------
// Umbra
// (c) 2023 Thomas Neumann
//---------------------------------------------------------------------------
namespace factDB {
//---------------------------------------------------------------------------
/// An aligned storage array that can be used to create uninitialized objects
template <class T>
struct RawObjectStorage {
   /// The backing storage
   alignas(T) std::byte buffer[sizeof(T)];

   /// Access
   T* get() { return std::launder(reinterpret_cast<T*>(buffer)); }
   const T* get() const { return std::launder(reinterpret_cast<const T*>(buffer)); }
   /// Access
   T* operator->() { return std::launder(reinterpret_cast<T*>(buffer)); }
   /// Access
   T& operator*() { return *std::launder(reinterpret_cast<T*>(buffer)); }
};
//---------------------------------------------------------------------------
} // namespace factDB
//---------------------------------------------------------------------------
