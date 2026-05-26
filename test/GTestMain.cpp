#include "factDB/infra/threading/LocalState.hpp"
#include "test/factDB/util/TestWriter.hpp"
#include <gtest/gtest.h>

int main(int argc, char** argv) {
   factDB::LocalState mainThreadState; // alive for entire test run
   testing::InitGoogleTest(&argc, argv);
   auto res = RUN_ALL_TESTS();

   if (res != 0)
      return res;

   factDB::TestWriter::generateCompilingTestCases();

   return res;
}