#include "environment.h"
#include <chrono>

namespace lox
{
namespace internal
{
class EnvironmentImplementation
{
public:
  std::optional<Environment> enclosing{};
  std::map<std::string, Value> values;
};
} // namespace internal

void
define_globals (Environment &global)
{
  global.define (
      "clock",
      Callable (
          [] (const std::vector<Value> &) {
            return Value{ static_cast<double> (
                std::chrono::duration_cast<std::chrono::milliseconds> (
                    std::chrono::system_clock::now ().time_since_epoch ())
                    .count ()) };
          },
          0));
}

Environment::Environment ()
    : pimpl (std::make_shared<internal::EnvironmentImplementation> ())
{
}

Environment
Environment::enclose (Environment enclosing)
{
  Environment env;
  env.pimpl->enclosing = enclosing;
  return env;
}

void
Environment::define (std::string name, Value value)
{
  // N.B. Use the access operator that will always overwrite.
  pimpl->values[name] = value;
}

const Value &
Environment::operator[] (const Token &name) const
{
  if (auto it = pimpl->values.find (name.lexeme); it != pimpl->values.end ())
    return it->second;

  if (pimpl->enclosing)
    return (*pimpl->enclosing)[name];

  throw RunTimeError (name, "Undefined variable '" + name.lexeme + "'.");
}

Value &
Environment::operator[] (const Token &name)
{
  const Environment &const_env = *this;
  return const_cast<Value &> (const_env[name]);
}

} // namespace lox
