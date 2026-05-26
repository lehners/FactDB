#ifndef H_factdb_queryc_writer_container_functorcontainer
#define H_factdb_queryc_writer_container_functorcontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include <functional>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct FunctorContainer : public Container {
   std::function<void(FileWriter&)> functor;
   FunctorContainer(std::function<void(FileWriter&)>&& func) : Container(CT_Functor), functor(std::move(func)){};
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_functorcontainer