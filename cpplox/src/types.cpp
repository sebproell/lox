#include "types.h"

namespace lox
{

struct LiteralVisitor
{
  std::string
  operator() (const std::monostate &) const
  {
    return "nil";
  }
  std::string
  operator() (const std::string &s) const
  {
    return s;
  }
  std::string
  operator() (const double &d) const
  {
    return std::to_string (d);
  }
};

std::string
to_string (const Literal &l)
{
  return std::visit (LiteralVisitor{}, l);
}
} // namespace lox
