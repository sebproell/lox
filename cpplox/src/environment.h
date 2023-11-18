#pragma once

#include <map>
#include <memory>
#include <string>

#include "error.h"
#include "types.h"

namespace lox
{
namespace internal
{
class EnvironmentImplementation;
}

/**
 * A reference-counted container for symbols.
 */
class Environment
{
public:
  Environment ();

  static Environment enclose (Environment enclosing);

  void define (std::string name, Value value);

  [[nodiscard]] const Value &operator[] (const Token &name) const;

  Value &operator[] (const Token &name);

private:
  std::shared_ptr<internal::EnvironmentImplementation> pimpl;
};

/**
 * Add all global symbols to the environment
 */
void define_globals (Environment &global);
} // namespace lox
