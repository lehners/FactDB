#pragma once // QueryImplementationGenerator.cpp(43:32)

#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationsMacro.hpp" // QueryImplementationGenerator.cpp(45:104)

namespace factDB::generated { // QueryImplementationGenerator.cpp(47:49)
enum Query { // QueryImplementationGenerator.cpp(48:32)
   dblp_acyclic_201_00 // QueryImplementationGenerator.cpp(49:64)
}; // QueryImplementationGenerator.cpp(50:22)

template <factDB::queryc::ExecutionMode execution, Query q> // QueryImplementationGenerator.cpp(52:79)
struct QuerySelector; // QueryImplementationGenerator.cpp(53:41)

// QUERY_IMPLEMENTATION(CodegenFlatpicb, dblp_acyclic_201_00); // QueryImplementationGenerator.cpp(56:136)
// QUERY_IMPLEMENTATION(CodegenFactorizedpicb, dblp_acyclic_201_00); // QueryImplementationGenerator.cpp(56:136)

inline std::vector<std::tuple<factDB::queryc::ExecutionMode, std::string, bool>> queries = {
   // QueryImplementationGenerator.cpp(59:112)
   {queryc::CodegenFlat, "dblp_acyclic_201_00", 30}, // QueryImplementationGenerator.cpp(61:130)
   {queryc::CodegenFactorized, "dblp_acyclic_201_00", 30}, // QueryImplementationGenerator.cpp(61:130)
}; // QueryImplementationGenerator.cpp(63:22)

} // namespace factDB::generated
