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

  Environment (Environment *enclosing);

  void define (std::string name, Value value);

  [[nodiscard]] const Value &operator[] (const Token &name) const;

  Value &operator[] (const Token &name);

private:
  std::map<std::string, Value> values;
  Environment *enclosing{ nullptr };
};

/**
 * Add all global symbols to the environment
 */
void define_globals (Environment &global);
} // namespace lox
