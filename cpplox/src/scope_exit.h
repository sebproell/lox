#pragma once

#include <functional>
#include <utility>

namespace lox
{
struct ScopeExit
{
  ScopeExit (std::function<void ()> callback) : callback (std::move (callback))
  {
  }

  ~ScopeExit () { callback (); }

private:
  std::function<void ()> callback;
};
} // namespace lox
