#ifndef H_FACTDB_TYPE_HPP
#define H_FACTDB_TYPE_HPP

#include "factDB/infra/Config.hpp"
#include <cstdint>
#include <ostream>
#include <string>
#include <typeinfo>

namespace factDB::schemac {

struct Type {
   // Type class
   enum Class : uint8_t {
      KUndefined,
      KInteger,
      KUInt64,
      KTimestamp,
      KDate,
      KNumeric,
      KChar,
      KVarchar,
      KBool,
   };
   // The Type class
   Class tclass;
   // The Type argument (if any)
   uint32_t length = 0;
   uint32_t precision = 0;

   Class getClass() const { return tclass; }

   constexpr Type() : tclass(KUndefined) {}
   constexpr explicit Type(Class classP, uint32_t lengthP = 0, uint32_t precisionP = 0) : tclass(classP), length(lengthP), precision(precisionP) {} // NOLINT(bugprone-easily-swappable-parameters)

   // Static methods to construct a column
   static Type Bool() { return Type{KBool}; }
   static Type Integer() { return Type{KInteger}; }
   static Type UInt64() { return Type{KUInt64}; }
   static Type Date() { return Type{KDate}; }
   static Type Timestamp() { return Type{KTimestamp}; }
   static Type Numeric(unsigned length, unsigned precision) { return Type{KNumeric, length, precision}; } // NOLINT
   static Type Char(unsigned length) { return Type{KChar, length}; }
   static Type Varchar(unsigned length) { return Type{KVarchar, length}; }

   [[nodiscard]] std::string toString() const {
      switch (tclass) {
         case KBool:
            return "Bool";
         case KInteger:
            return "Integer";
         case KVarchar:
            return "Varchar<" + std::to_string(length) + ">";
         case KChar:
            return "Char<" + std::to_string(length) + ">";
         case KNumeric:
            return "Numeric" + std::to_string(length) + "_" + std::to_string(precision) + "";
         case KTimestamp:
            return "Timestamp";
         case KDate:
            return "Date";
         case KUInt64:
            return "UInt64";
         case KUndefined:
         default:
            return "Unknown";
      }
   }

   [[nodiscard]] std::string toConstructor() const {
      switch (tclass) {
         case KBool: return "Bool()";
         case KInteger: return "Integer()";
         case KUInt64: return "UInt64()";
         case KDate: return "Date()";
         case KTimestamp: return "Timestamp()";
         case KNumeric: return std::string("Numeric(") + std::to_string(length) + ", " + std::to_string(precision) + ")";
         case KChar: return std::string("Char(") + std::to_string(length) + ")";
         case KVarchar: return std::string("Varchar(") + std::to_string(length) + ")";
         case KUndefined: unreachable();
         default: return "Unknown";
      }
   }

   [[nodiscard]] static Type fromString(const std::string& str) {
      if (str == "Bool") {
         return Bool();
      } else if (str == "Integer") {
         return Integer();
      } else if (str == "UInt64") {
         return UInt64();
      } else if (str == "Timestamp") {
         return Timestamp();
      } else if (str == "Date") {
         return Date();
      } else if (str.compare(0, 4, "Char") == 0) {
         int length = std::stoi(str.substr(5));
         return Char(length);
      } else if (str.compare(0, 7, "Numeric") == 0) {
         size_t end;
         int length = std::stoi(str.substr(8), &end);
         int precision = std::stoi(str.substr(8 + end + 1));
         return Numeric(length, precision);
      } else if (str.compare(0, 7, "Varchar") == 0) {
         int length = std::stoi(str.substr(8));
         return Varchar(length);
      } else {
         throw std::bad_typeid();
      }
   }
   // ---------------------------------------------------------------------------------------------------
   // Get Type name
   [[nodiscard]] const char* name() const {
      switch (tclass) {
         case KBool: return "Bool";
         case KInteger: return "Integer";
         case KUInt64: return "UInt64";
         case KTimestamp: return "Timestamp";
         case KDate: return "Date";
         case KNumeric: return "Numeric";
         case KChar: return "Character";
         case KVarchar: return "Varchar";
         case KUndefined:
         default: return "Unknown";
      }
   }

   bool operator==(Type other) const {
      switch (tclass) {
         case KTimestamp:
         case KDate:
         case KInteger:
         case KUInt64:
         case KBool:
            return tclass == other.tclass;
         case KVarchar:
         case KChar:
            return tclass == other.tclass && length == other.length;
         case KNumeric:
            return tclass == other.tclass && length == other.length && precision == other.precision;
         default:
            return false;
      }
   }
};

} // namespace factDB::schemac

#endif // H_FACTDB_TYPE_HPP