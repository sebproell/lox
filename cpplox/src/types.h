#pragma once
#include <cstddef>
#include <functional>
#include <string>
#include <variant>

namespace lox
{
struct Callable;

/**
 * The types of literals that may occur in code
 */
using Literal = std::variant<std::nullptr_t, std::string, bool, double>;

/**
 * The result of interpreting a syntax tree.
 */
using Value
    = std::variant<std::nullptr_t, std::string, bool, double, Callable>;

/**
 * String representation of a literal.
 */
std::string to_string (const Literal &l);

/**
 * A callable object. As all types in this project, this type behaves like a
 * value. Instead of implementing a virtual interface, we use type erasure.
 */
struct Callable
{
  /**
   * Implicitly create callables from compatible types.
   */
  template <typename T,
            std::enable_if_t<!std::is_same_v<Callable, std::decay_t<T> >, int>
            = 0>
  Callable (T &&in, unsigned arity)
      : fn (std::forward<T> (in)), my_arity (arity)
  {
  }

  auto
  operator() (const std::vector<Value> &args) const
  {
    return fn (args);
  }

  [[nodiscard]] unsigned
  arity () const
  {
    return my_arity;
  }

private:
  std::function<Value (const std::vector<Value> &args)> fn;
  unsigned my_arity{};
};

} // namespace lox
