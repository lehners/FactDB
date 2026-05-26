#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include <functional>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::generated {
// ---------------------------------------------------------------------------------------------------
std::string getSchemaIdentifier();
std::vector<std::string> getRequiredRelations();
extern "C" void query(factDB::Database& database, std::ostream& out_stream, queryc::PerformanceRecorderExecution pre);
// ---------------------------------------------------------------------------------------------------
using QueryFunctionType = std::function<void(factDB::Database&, std::ostream&, queryc::PerformanceRecorderExecution)>;
using RequiredRelationsFunctionType = std::function<std::vector<std::string>()>;
using SchemaIdentifierFunctionType = std::function<std::string()>;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::generated
// ---------------------------------------------------------------------------------------------------