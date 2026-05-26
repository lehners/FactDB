#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/SettingParser.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
enum ExecutionMode {
   CodegenFlat,
   CodegenFactorized,
   CodegenFactorizedNaive,
   Materialized,
   PushModel,
   Graphviz,
   FactorizationGain,
   FTreeComposition,
   FTreeDifference,
   FTreePredecessorDifference,
   Costs,
   FlatSizes,
   SQL,
   UmbraLocal,
   UmbraServer
};
// ---------------------------------------------------------------------------------------------------
static constexpr auto executionModeParser = settinghelper::makeEnumParser(
   std::tuple{CodegenFlat, "CodegenFlat", 'c'},
   std::tuple{CodegenFactorized, "CodegenFactorized", 'f'},
   std::tuple{CodegenFactorizedNaive, "CodegenFactorized", 'n'},
   std::tuple{Materialized, "Materialized", 'm'},
   std::tuple{PushModel, "PushModel", 'p'},
   std::tuple{Graphviz, "Graphviz", 'g'},
   std::tuple{FactorizationGain, "FactorizationGain", 'i'},
   std::tuple{FTreeComposition, "FTreeComposition", 'b'},
   std::tuple{FTreeDifference, "FTreeDifference", 'd'},
   std::tuple{FTreePredecessorDifference, "FTreePredecessorDifference", 'r'},
   std::tuple{Costs, "Costs", 'C'},
   std::tuple{FlatSizes, "FlatSizes", 'v'},
   std::tuple{SQL, "SQL", 's'},
   std::tuple{UmbraLocal, "UmbraLocal", 'u'},
   std::tuple{UmbraServer, "UmbraServer", 'z'});
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
