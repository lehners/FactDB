#include "factDB/infra/Setting.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include <iostream>
// ---------------------------------------------------------------------------------------------------
int main() {
#ifdef HAS_SQLITE3
   factDB::SettingBase::getSetting<bool>("opt.oracle.import.db")->setTemporary(!true);
   factDB::opt::CardinalityOracleDBCached::importFileIntoDB();
#else
   std::cout << "No SQLLite available" << std::endl;
#endif
   return 0;
}
// ---------------------------------------------------------------------------------------------------
