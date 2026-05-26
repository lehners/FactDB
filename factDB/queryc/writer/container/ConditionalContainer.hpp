#ifndef H_factdb_queryc_writer_container_conditionalcontainer
#define H_factdb_queryc_writer_container_conditionalcontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include "factDB/queryc/writer/FWContainer.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct ConditionalContainer : public Container {
   bool condition;
   FWContainer trueCase;
   FWContainer falseCase;
   ConditionalContainer(bool cond, FWContainer&& tCase);
   ConditionalContainer(bool cond, FWContainer&& tCase, FWContainer&& fCase) : Container(CT_Conditional), condition(cond), trueCase(std::forward<FWContainer>(tCase)), falseCase(std::forward<FWContainer>(fCase)) {}
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_conditionalcontainer