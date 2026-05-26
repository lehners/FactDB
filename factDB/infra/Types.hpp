#ifndef H_FACTDB_FACT_DB_INFRA_TYPES_HPP
#define H_FACTDB_FACT_DB_INFRA_TYPES_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/Bool.hpp"
#include "factDB/infra/types/Char.hpp"
#include "factDB/infra/types/Integer.hpp"
#include "factDB/infra/types/Numeric.hpp"
#include "factDB/infra/types/Timestamp.hpp"
#include "factDB/infra/types/UInt.hpp"
#include "factDB/infra/types/Varchar.hpp"
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
template <class U, template <class> class T>
struct isDerivedFrom {
   private:
   template <class V>
   static decltype(static_cast<const T<V>&>(std::declval<U>()), std::true_type{})
   test(const T<V>&);

   static std::false_type test(...);

   public:
   static constexpr bool value = decltype(isDerivedFrom::test(std::declval<U>()))::value;
};
// ---------------------------------------------------------------------------------------------------
template <class T, template <unsigned> class Template>
struct is_specialization_size : std::false_type {};
// ---------------------------------------------------------------------------------------------------
template <template <unsigned> class Template, unsigned kArgs>
struct is_specialization_size<Template<kArgs>, Template> : std::true_type {};
// ---------------------------------------------------------------------------------------------------
} // anonymous namespace
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <typename T>
using isFactDBUIntType = isDerivedFrom<T, UIntBase>;
// ---------------------------------------------------------------------------------------------------
template <typename T>
using isFactDBIntType = isDerivedFrom<T, IntBase>;
// ---------------------------------------------------------------------------------------------------
template <typename T>
using isFactDBTimestampType = std::is_same<T, Timestamp>;
// ---------------------------------------------------------------------------------------------------
template <typename T>
using isFactDBNumericType = std::disjunction<std::is_same<T, Numeric18_2>>;
// ---------------------------------------------------------------------------------------------------
template <typename T>
using isFactDBDateType = std::is_same<T, Date>;
// ---------------------------------------------------------------------------------------------------
template <typename T>
using isFactDBStringType = is_specialization_size<T, Varchar>;
// ---------------------------------------------------------------------------------------------------
template <typename T>
using isFactDBCharType = is_specialization_size<T, Char>;
// ---------------------------------------------------------------------------------------------------
template <typename T>
using isFactDBType = std::disjunction<isFactDBIntType<T>, isFactDBUIntType<T>, isFactDBNumericType<T>, isFactDBTimestampType<T>, isFactDBStringType<T>, isFactDBCharType<T>, isFactDBDateType<T>>;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_TYPES_HPP