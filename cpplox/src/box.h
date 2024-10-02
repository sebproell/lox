#pragma once

#include <memory>

namespace lox
{
/**
 * Box a type T into a heap-allocated structure. The Box type allows to define
 * recursive data structures with value semantics.
 */
template <typename T> struct Box
{
  std::unique_ptr<T> impl;

public:
  Box (T &&obj) : impl (new T (std::move (obj))) {}
  Box (const T &obj) : impl (new T (obj)) {}

  Box (const Box &other) : Box (*other.impl) {}
  Box &
  operator= (const Box &other)
  {
    *impl = *other.impl;
    return *this;
  }

  ~Box () = default;

  T &
  operator* ()
  {
    return *impl;
  }

  const T &
  operator* () const
  {
    return *impl;
  }

  T *
  operator->()
  {
    return impl.get ();
  }

  const T *
  operator->() const
  {
    return impl.get ();
  }
};
} // namespace lox
