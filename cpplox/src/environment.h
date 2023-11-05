#pragma once

#include <map>
#include <string>

#include "error.h"
#include "types.h"

namespace lox
{
class Environment
{
public:
  Environment () = default;

  Environment (Environment *enclosing) : enclosing (enclosing) {}

  void
  define (std::string name, Value value)
  {
    // N.B. Use the access operator that will always overwrite.
    values[name] = value;
  }

  [[nodiscard]] const Value &
  operator[] (const Token &name) const
  {
    if (auto it = values.find (name.lexeme); it != values.end ())
      return it->second;

    if (enclosing != nullptr)
      return (*enclosing)[name];

    throw RunTimeError (name, "Undefined variable '" + name.lexeme + "'.");
  }

  Value &
  operator[] (const Token &name)
  {
    const Environment &const_env = *this;
    return const_cast<Value &> (const_env[name]);
  }

private:
  std::map<std::string, Value> values;
  Environment *enclosing{ nullptr };
};
} // namespace lox
