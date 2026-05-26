# ---------------------------------------------------------------------------
# FACTDB
# ---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

# set(CMAKE_C_COMPILER "gcc")
# set(CMAKE_CXX_COMPILER "/usr/lib/ccache/g++")

# set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "/usr/lib/ccache/clang++")

# ---------------------------------------------------------------------------

# cmake_minimum_required(VERSION 3.28) todo for exclude from all
project(factDB)

# ---------------------------------------------------------------------------
# Environment
# ---------------------------------------------------------------------------

# add_compile_definitions(ENABLE_COVERAGE=1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/vendor/")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # add_definitions(-DDEBUG_MODE=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_definitions(-DNDEBUG=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  add_definitions(-DNDEBUG=1)
endif()

# ---------------------------------------------------------------------------
# Install pre-commit hooks
# ---------------------------------------------------------------------------

include("${CMAKE_CURRENT_SOURCE_DIR}/makeutil/InstallCommitHooks.cmake")

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/vendor)
if (APPLE)
  include("${CMAKE_CURRENT_SOURCE_DIR}/vendor/ranges_v3.cmake")
endif ()
include("${CMAKE_CURRENT_SOURCE_DIR}/vendor/nlohmann_json.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/vendor/bundled-gtest.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/vendor/fmt.cmake")
#find_package(GTest REQUIRED)
#find_package(fmt REQUIRED)
find_package(TBB REQUIRED)
find_package(TBB COMPONENTS tbbmalloc tbbmalloc_proxy tbb_preview)
find_package(PostgreSQL REQUIRED) # for umbra server connector
find_package(SQLite3)

if (SQLite3_FOUND)
  add_definitions(-DHAS_SQLITE3)
endif()

# ---------------------------------------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -Wall -Wextra -Wshadow")
if (APPLE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fno-omit-frame-pointer")
else()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fno-omit-frame-pointer -fsanitize=address")
endif()
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O3 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")


# ---------------------------------------------------------------------------
# Parser
# ---------------------------------------------------------------------------
# todo maybe fix this for mac like in imlab
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
include("${CMAKE_SOURCE_DIR}/factDB/parser/local.cmake")


configure_file(
    factDB/config.h.in factDB/config.h)
# ---------------------------------------------------------------------------
# Includes
# ---------------------------------------------------------------------------

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------

add_subdirectory(factDB/gen/tables)
add_subdirectory(factDB/gen/query)
add_subdirectory(test/factDB)

set(SRC_CC
    factDB/newftree/generator/util/FIteratorGenerator.cpp
    factDB/Database.cpp
    factDB/SQLExecution.cpp
    factDB/algebra/BinaryOperator.cpp
    factDB/algebra/Count.cpp
    factDB/algebra/Reference.cpp
    factDB/algebra/CrossProduct.cpp
    factDB/algebra/InnerJoin.cpp
    factDB/algebra/Print.cpp
    factDB/algebra/Selection.cpp
    factDB/algebra/TableScan.cpp
    factDB/algebra/UnaryOperator.cpp
    factDB/algebra/translator/ChainingHashtableTranslator.cpp
    factDB/algebra/translator/FactorizedTableTranslator.cpp
    factDB/algebra/translator/TimingTranslator.cpp
    factDB/algebra/translator/VectorTranslator.cpp
    factDB/algebra/visitors/AlgebraVisitor.cpp
    factDB/algebra/visitors/factorized/FactorizedTreeDeriver.cpp
    factDB/algebra/visitors/factorized/FTreeDifference.cpp
    factDB/algebra/visitors/factorized/FTreePredecessorDifference.cpp
    factDB/algebra/visitors/factorized/FTreeComposition.cpp
    factDB/algebra/visitors/CodegenExpanded.cpp
    factDB/algebra/visitors/CodegenFactorized.cpp
    factDB/algebra/visitors/FactorizationGain.cpp
    factDB/algebra/visitors/FindPipelineBreaker.cpp
    factDB/algebra/visitors/GraphvizQueryPlan.cpp
    factDB/algebra/visitors/MaterializedExecution.cpp
    factDB/algebra/visitors/PrettyQueryPlanPrinter.cpp
    factDB/algebra/visitors/PushModelExecution.cpp
    factDB/algebra/visitors/QueryPlanWriter.cpp
    factDB/algebra/visitors/SQLGenerator.cpp
    factDB/algebra/visitors/SQLGeneratorSemijoinGroupBy.cpp
    factDB/algebra/visitors/umbra/UmbraConnector.cpp
    factDB/algebra/visitors/umbra/UmbraServerConnector.cpp
    factDB/algebra/visitors/umbra/UmbraVisitor.cpp
    factDB/infra/BitSet.cpp
    factDB/infra/list/ListTypeEnum.cpp
    factDB/infra/Expression.cpp
    factDB/infra/IUSet.cpp
    factDB/infra/RuntimeException.cpp
    factDB/infra/Setting.cpp
    factDB/infra/SettingParser.cpp
    factDB/infra/threading/ThreadLocal.cpp
    factDB/infra/UnionFind.cpp
    factDB/infra/types/Bool.cpp
    factDB/infra/types/Char.cpp
    factDB/infra/types/Integer.cpp
    factDB/infra/types/Numeric.cpp
    factDB/infra/types/RuntimeValue.cpp
    factDB/infra/types/Timestamp.cpp
    factDB/infra/types/UInt.cpp
    factDB/infra/types/Varchar.cpp
    factDB/opt/CreateJoins.cpp
    factDB/opt/Optimizer.cpp
    factDB/opt/OptimizerRepresentation.cpp
    factDB/opt/OptimizerRepresentationGenerator.cpp
    factDB/opt/Plan.cpp
    factDB/opt/FlattenJoins.cpp
    factDB/opt/PredecessorDetector.cpp
    factDB/opt/PredicatePushDown.cpp
    factDB/opt/QueryGraph.cpp
    factDB/opt/QuerySignature.cpp
    factDB/opt/OptimizeBuildSide.cpp
    factDB/opt/estimators/FactorizedEstimatorOnlyCardinality.cpp
    factDB/opt/algs/AnyPlan.cpp
    factDB/opt/algs/DPFact.cpp
    factDB/opt/algs/DPSize.cpp
    factDB/opt/algs/FixedPlan.cpp
    factDB/opt/algs/OptimizerAlgorithms.cpp
    factDB/opt/estimators/BaseEstimator.cpp
    factDB/opt/estimators/EstimateAlgebraPlan.cpp
    factDB/opt/oracle/BaseOracle.cpp
    factDB/opt/oracle/CardinalityOracleCSV.cpp
    factDB/opt/oracle/CardinalityOracleDB.cpp
    factDB/opt/oracle/CardinalityOracleDBCached.cpp
    factDB/opt/estimators/FactorizedEstimator.cpp
    factDB/opt/estimators/FlatEstimator.cpp
    factDB/parser/AST.cpp
    factDB/parser/SchemaParseContext.cpp
    factDB/parser/SemanticAnalysis.cpp
    factDB/parser/SemanticAnalysisExpression.cpp
    factDB/parser/SemanticAnalysisQuery.cpp
    factDB/parser/gen/parser.cc
    factDB/parser/gen/scanner.cc
    factDB/queryc/CxxCache.cpp
    factDB/queryc/queryDispatcher/runtime/KnownQueries.cpp
    factDB/queryc/queryDispatcher/runtime/QueryDispatcher.cpp
    factDB/queryc/queryDispatcher/compiletime/QueryImplementationGenerator.cpp
    factDB/queryc/CxxWrapper.cpp
    factDB/queryc/NewFileWriter.cpp
    factDB/queryc/PerformanceRecord.cpp
    factDB/queryc/QueryCompiler.cpp
    factDB/queryc/QueryHandle.cpp
    factDB/queryc/QueryParseContext.cpp
    factDB/queryc/writer/FWContainer.cpp
    factDB/queryc/writer/FWUtil.cpp
    factDB/queryc/writer/FileWriter.cpp
    factDB/queryc/writer/container/ListContainer.cpp
    factDB/queryc/writer/container/ReferenceContainer.cpp
    factDB/queryc/writer/container/TrivialContainer.cpp
    factDB/schemac/SchemaCompiler.cpp
    factDB/statement/CopyStatement.cpp
    factDB/statement/CreateTableStatement.cpp
    factDB/statement/QueryStatement.cpp
    factDB/util/CommandLine.cpp
    factDB/util/DatabaseLoadUtil.cpp
    factDB/util/JsonHandler.cpp
    factDB/util/Perfetto.cpp
    factDB/infra/alloc/Mmap.cpp
    factDB/infra/util/LineSortingStream.cpp
    factDB/infra/alloc/BumpAllocator.cpp
    factDB/infra/alloc/FastAlloc.cpp
    factDB/newftree/FTree.cpp
    factDB/newftree/FNode.cpp
    factDB/newftree/FIterator.cpp
    factDB/newftree/generator/util/FNodeGenerator.cpp
    factDB/newftree/generator/util/FGenUtil.cpp
    factDB/newftree/generator/FGen.cpp
    factDB/newftree/generator/GenerateFTree.cpp
    factDB/newftree/generator/fnode/FNodeSettings.cpp
)

set(TEST_CC
    test/GTestMain.cpp
    test/factDB/TestDatabase.cpp
    # test/factDB/algebra/translator/TestCleanedupFactorizedTableTranslator.cpp
    test/factDB/algebra/translator/TestFactorizedTableTranslator.cpp
    test/factDB/algebra/visitors/TestCodegenExpanded.cpp
    test/factDB/algebra/visitors/TestCodegenFactorized.cpp
    test/factDB/algebra/visitors/TestMaterializedExecution.cpp
    test/factDB/algebra/visitors/TestPipelineVisitor.cpp
    test/factDB/algebra/visitors/TestPushModelExecution.cpp
    test/factDB/infra/DoNotDestruct.cpp
    test/factDB/infra/TestBitSet.cpp
    test/factDB/infra/TestChainingHashtable.cpp
    test/factDB/infra/TestChunkedList.cpp
    test/factDB/infra/TestTypes.cpp
    test/factDB/infra/threading/TestThreadLocal.cpp
    test/factDB/infra/TestUnionFind.cpp
    test/factDB/infra/list/TestInlineList.cpp
    test/factDB/infra/list/TestFastDeque.cpp
    test/factDB/opt/QueryGraphBuilder.cpp
    test/factDB/opt/TestCardinalityOracle.cpp
    test/factDB/opt/TestDP.cpp
    test/factDB/opt/TestOptimizer.cpp
    test/factDB/opt/TestPlan.cpp
    test/factDB/opt/TestQueryGraph.cpp
    test/factDB/parser/TestQueryParser.cpp
    test/factDB/parser/TestSchemaParser.cpp
    test/factDB/queryc/TestQueryCompiler.cpp
    test/factDB/util/TestCommandLine.cpp
    test/factDB/util/TestJsonHandler.cpp
    test/factDB/util/TranslationTest.cpp
    test/infra/TestAllocatorPooled.cpp
    test/infra/util/TestHash.cpp
    test/infra/TestBumpAllocator.cpp
    test/factDB/util/TestTaggedPointer.cpp
    test/factDB/newftree/TestFIterator.cpp
    test/factDB/util/TestWriter.cpp
)

file(GLOB FACTSQL_FILES
    "cmake-build-relwithdebinfo/gen/queries/query180.cpp"
)


if (FACTSQL_FILES)
  add_compile_definitions(HAVE_QUERY_CPP_FILE=1)
else ()
  add_compile_definitions(HAVE_QUERY_CPP_FILE=0)
endif ()

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------

add_library(factDB STATIC ${SRC_CC} ${TABLE_FILES})
add_dependencies(factDB parser)
IF (APPLE)
IF (SQLite3_FOUND)
target_link_libraries(factDB fmt::fmt TBB::tbb range-v3 nlohmann_json::nlohmann_json PostgreSQL::PostgreSQL TBB::tbbmalloc SQLite::SQLite3)
ELSE ()
target_link_libraries(factDB fmt::fmt TBB::tbb range-v3 nlohmann_json::nlohmann_json PostgreSQL::PostgreSQL TBB::tbbmalloc)
ENDIF ()
ELSE ()
  IF (SQLite3_FOUND)
    target_link_libraries(factDB fmt::fmt TBB::tbb nlohmann_json::nlohmann_json PostgreSQL::PostgreSQL TBB::tbbmalloc SQLite::SQLite3)
  ELSE ()
    target_link_libraries(factDB fmt::fmt TBB::tbb nlohmann_json::nlohmann_json PostgreSQL::PostgreSQL TBB::tbbmalloc)
  ENDIF ()
ENDIF ()
IF (EXISTS ../factDB/gen/*.cpp)
  FILE(GLOB factDBgenSRC factDB/gen/*.cpp)
  message(STATUS ${factDBgenSRC})
  add_library(factDBgen STATIC ${factDBgenSRC})
  target_link_libraries(factDBgen fmt::fmt TBB::tbb range-v3)
ENDIF ()

add_executable(genTblFromCSV factDB/tools/GenTblFiles.cpp)
target_link_libraries(genTblFromCSV factDB)

add_executable(generateTables factDB/tools/GenTables.cpp)
target_link_libraries(generateTables factDB)

add_executable(factSQL EXCLUDE_FROM_ALL factDB/main.cpp test/factDB/util/TranslationTest.cpp ${FACTSQL_FILES})
target_link_libraries(factSQL factDB TBB::tbbmalloc nlohmann_json::nlohmann_json)

add_executable(ImportEstimateCSV2DB factDB/tools/ImportEstimateCSV2DB.cpp)
target_link_libraries(ImportEstimateCSV2DB factDB)

# ---------------------------------------------------------------------------
# Benchmarks
# ---------------------------------------------------------------------------

add_executable(benchArtificial bench/artificial/bench.cpp bench/artificial/QueryTrees.cpp bench/artificial/DataGen.cpp ${QUERY_FILES})
target_link_libraries(benchArtificial factDB)
target_compile_options(benchArtificial PRIVATE -Wno-shadow -Wno-unused-variable)

add_executable(benchAll bench/bench.cpp ${QUERY_FILES})
target_link_libraries(benchAll factDB)
target_compile_options(benchAll PRIVATE -Wno-shadow -Wno-unused-variable)

add_executable(benchWINF bench/WorldIsNotFlatQuery.cpp)
target_link_libraries(benchWINF factDB)
target_compile_options(benchWINF PRIVATE -Wno-shadow -Wno-unused-variable)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

enable_testing()
add_executable(tester ${TEST_CC})
add_dependencies(tester factDB)
target_link_libraries(
    tester
    factDB
    GTest::GTest
    fmt::fmt
)

include(GoogleTest)
gtest_discover_tests(tester DISCOVERY_MODE PRE_TEST)
# ---------------------------------------------------------------------------

add_executable(test_generated
        test/GTestMain.cpp
        test/factDB/GeneratedTests.cpp
        test/factDB/util/TestWriter.cpp
        ${TEST_QUERY_FILES}
)
add_dependencies(test_generated factDB)
target_link_libraries(
        test_generated
        factDB
        GTest::GTest
        fmt::fmt
)

gtest_discover_tests(test_generated DISCOVERY_MODE PRE_TEST)

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

message(STATUS "[COMPILER] settings")
message(STATUS "    CMAKE_C_COMPILER            = ${CMAKE_C_COMPILER}")
message(STATUS "    CMAKE_CXX_COMPILER          = ${CMAKE_CXX_COMPILER}")
message(STATUS "    CMAKE_CXX_COMPILER_AR       = ${CMAKE_CXX_COMPILER_AR}")
message(STATUS "    CMAKE_CXX_COMPILER_RANLIB   = ${CMAKE_CXX_COMPILER_RANLIB}")
message(STATUS "    Using BUILD MODE            = ${CMAKE_BUILD_TYPE}")
message(STATUS "    Using CONF TYPES            = ${CMAKE_CONFIGURATION_TYPES}")
