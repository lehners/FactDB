#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/schemac/Type.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Bool;
// ---------------------------------------------------------------------------------------------------
struct RuntimeString {
   public:
   const char* ptr;
   uint64_t len;

   /// Get the length of the RuntimeString
   [[nodiscard]] uint64_t getLength() const noexcept { return len; }
   /// Get the ptr of the RuntimeString
   [[nodiscard]] const char* getPointer() const noexcept { return ptr; }
   /// Get a string_view to the RuntimeString
   [[nodiscard]] std::string_view getStringView() const noexcept { return {ptr, len}; }
   /// compare
   [[nodiscard]] bool operator==(const RuntimeString& other) const { return getStringView() == other.getStringView(); }
   [[nodiscard]] bool operator!=(const RuntimeString& other) const { return getStringView() != other.getStringView(); }
   [[nodiscard]] bool operator<(const RuntimeString& other) const { return getStringView() < other.getStringView(); }
   [[nodiscard]] bool operator<=(const RuntimeString& other) const { return getStringView() <= other.getStringView(); }
   [[nodiscard]] bool operator>=(const RuntimeString& other) const { return getStringView() >= other.getStringView(); }
   [[nodiscard]] bool operator>(const RuntimeString& other) const { return getStringView() > other.getStringView(); }
};
// ---------------------------------------------------------------------------------------------------
struct RuntimeValue {
   private:
   // the pointer to the actual value
   std::array<std::byte, 16> rawValue;
   // The buffer for strings > shortStringLimit
   std::unique_ptr<char[]> stringValue = nullptr;
   // the type of the value
   schemac::Type type;

   public:
   /// Get a value
   template <typename T>
      requires(std::is_trivial_v<T> && sizeof(T) <= 16)
   [[nodiscard]] T get() const {
      T value;
      __builtin_memcpy(&value, rawValue.data(), sizeof(T));
      return value;
   }
   /// Set a value
   template <typename T>
      requires(std::is_trivial_v<T> && sizeof(T) <= 16)
   void set(T value) {
      __builtin_memcpy(rawValue.data(), &value, sizeof(T));
   }

   void setString(std::string_view input);

   void setType(schemac::Type t) { type = t; }
   [[nodiscard]] schemac::Type getType() const { return type; }

   public:
   RuntimeValue() = default;
   RuntimeValue(const RuntimeValue&);
   RuntimeValue(RuntimeValue&& other) noexcept = default;
   ~RuntimeValue() = default;

   RuntimeValue& operator=(const RuntimeValue& other);
   RuntimeValue& operator=(RuntimeValue&& other) noexcept = default;

   [[nodiscard]] uint64_t hash() const;
   /// Comparison
   [[nodiscard]] bool operator==(const RuntimeValue& n) const;
   /// Comparison
   [[nodiscard]] bool operator!=(const RuntimeValue& n) const { return !(*this == n); };
   /// Comparison
   [[nodiscard]] bool operator<(const RuntimeValue& n) const;
   /// Comparison
   [[nodiscard]] bool operator<=(const RuntimeValue& n) const;
   /// Comparison
   [[nodiscard]] bool operator>(const RuntimeValue& n) const;
   /// Comparison
   [[nodiscard]] bool operator>=(const RuntimeValue& n) const;

   [[nodiscard]] Bool operator||(const RuntimeValue& n) const;
   [[nodiscard]] Bool operator&&(const RuntimeValue& n) const;
};
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const RuntimeValue& val);
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------