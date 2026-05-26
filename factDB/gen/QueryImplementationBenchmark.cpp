#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationBenchmark.hpp" // QueryImplementationGenerator.cpp(71:115)

namespace factDB::generated { // QueryImplementationGenerator.cpp(73:57)

   void runBenchmarks(queryc::ExecutionMode execution, Database& db) { // QueryImplementationGenerator.cpp(75:95)
      auto qp = QueryProperties::fromCurrentSettings(); // QueryImplementationGenerator.cpp(76:77)
      if (execution == queryc::CodegenFlat && qp.getFlagUInt() == 30) benchmarkQuery<queryc::CodegenFlatpicb, dblp_acyclic_201_00>(db); // QueryImplementationGenerator.cpp(78:226)
      if (execution == queryc::CodegenFactorized && qp.getFlagUInt() == 30) benchmarkQuery<queryc::CodegenFactorizedpicb, dblp_acyclic_201_00>(db); // QueryImplementationGenerator.cpp(78:226)
   } // QueryImplementationGenerator.cpp(79:29)
} // namespace factDB::generated // QueryImplementationGenerator.cpp(80:60)
