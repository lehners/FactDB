// ---------------------------------------------------------------------------------------------------
#ifndef IMLAB_RUNTIMEEXCEPTION_H
#define IMLAB_RUNTIMEEXCEPTION_H
// ---------------------------------------------------------------------------------------------------
#include <exception>
#include <string>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
enum ErrorCode {
   RuntimeError,
   NotImplementedYet,
   InvalidNumberFormat,
   InvalidBoolFormat,
   InvalidTimestampFormat,
   UmbraTimout,
   UmbraOoM,
   DoNotGeneratePlan,
};
// ---------------------------------------------------------------------------------------------------
class RuntimeException : public std::exception {
   private:
   ErrorCode errorCode;
   /// The message that caused the exception
   std::string message;

   public:
   /// Constructor
   explicit RuntimeException(ErrorCode code, const std::string& message);
   /// Destructor
   ~RuntimeException() = default;
   /// Constructor
   RuntimeException(const RuntimeException&) = default;
   /// Constructor
   RuntimeException(RuntimeException&&) noexcept = default;
   /// Assignment
   RuntimeException& operator=(const RuntimeException&) = default;
   /// Assignment
   RuntimeException& operator=(RuntimeException&&) = default;

   /// Get the message
   [[nodiscard]] const std::string& getMessage() const { return message; }

   /// Get the message using the standard exception interface
   [[nodiscard]] const char* what() const noexcept override;

   /// Utility to catch any exception and convert it to a RuntimeMessage
   static RuntimeException ensure(std::exception_ptr exception);

   [[nodiscard]] constexpr ErrorCode getErrorCode() const { return errorCode; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif //IMLAB_RUNTIMEEXCEPTION_H
// ---------------------------------------------------------------------------------------------------
