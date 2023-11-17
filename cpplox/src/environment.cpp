#include "environment.h"
#include <chrono>

namespace lox
{
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

Environment::Environment (Environment *enclosing) : enclosing (enclosing) {}

void
Environment::define (std::string name, Value value)
{
  // N.B. Use the access operator that will always overwrite.
  values[name] = value;
}

const Value &
Environment::operator[] (const Token &name) const
{
  if (auto it = values.find (name.lexeme); it != values.end ())
    return it->second;

  if (enclosing != nullptr)
    return (*enclosing)[name];

  throw RunTimeError (name, "Undefined variable '" + name.lexeme + "'.");
}

Value &
Environment::operator[] (const Token &name)
{
  const Environment &const_env = *this;
  return const_cast<Value &> (const_env[name]);
}

} // namespace lox
