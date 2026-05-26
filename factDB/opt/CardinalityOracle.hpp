#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/oracle/CardinalityOracleCSV.hpp"
#include "factDB/opt/oracle/CardinalityOracleDB.hpp"
#include "factDB/opt/oracle/CardinalityOracleDBCached.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
#ifdef HAS_SQLITE3
using CardinalityOracle = CardinalityOracleDB;
#else
using CardinalityOracle = CardinalityOracleCSV;
#endif
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------