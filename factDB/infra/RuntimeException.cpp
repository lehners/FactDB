// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/RuntimeException.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
RuntimeException::RuntimeException(ErrorCode code, const std::string& msg) : errorCode(code) {
   switch (code) {
      case RuntimeError:
         message = "RuntimeError: " + msg;
         break;
      case NotImplementedYet:
         message = "Not yet implemented: " + msg;
         break;
      case InvalidNumberFormat:
         message = "Invalid Parameter Format: " + msg;
         break;
      case InvalidTimestampFormat:
         message = "Invalid Timestamp Format: " + msg;
         break;
      case InvalidBoolFormat:
         message = "Invalid Boolean Format: " + msg;
         break;
      case UmbraTimout:
         message = "Umbra Timeout: " + msg;
         break;
      case UmbraOoM:
         message = "Umbra went out of memory: " + msg;
         break;
      case DoNotGeneratePlan:
         message = "Setting to optimize plans is disabled." + msg;
         break;
   }
}
// ---------------------------------------------------------------------------------------------------
const char* RuntimeException::what() const noexcept
// Get the message using the standard exception interface
{
   return message.c_str();
}
// ---------------------------------------------------------------------------------------------------
RuntimeException RuntimeException::ensure(exception_ptr exception)
// Utility to catch any exception and convert them to a RuntimeException
{
   try {
      rethrow_exception(std::move(exception));
   } catch (RuntimeException& e) {
      return std::move(e);
   } catch (const std::exception& e) {
      return RuntimeException(RuntimeError, e.what());
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------