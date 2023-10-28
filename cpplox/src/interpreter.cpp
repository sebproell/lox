#include "interpreter.h"
#include "error.h"
#include "token.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <variant>

namespace lox
{
namespace
{

/**
 * The result of interpreting a syntax tree.
 */
using Value = std::variant<std::nullptr_t, double, bool, std::string>;

// overload visit with multiple lambdas
template <class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};
template <class... Ts> overloaded (Ts...) -> overloaded<Ts...>;

[[nodiscard]] double
numeric (const Value &value)
{
  // This call can never fail due to checks performed before-hand
  return std::get<double> (value);
}

bool
is_truthy (const Value &value)
{
  return std::visit (overloaded{ [] (const bool &b) { return b; },
                                 [] (const std::nullptr_t &) { return false; },
                                 [] (const auto &) { return true; } },
                     value);
}

struct is_equal_same_type
{
  template <typename T>
  bool
  operator() (const T &a, const T &b) const
  {
    return a == b;
  }
};

bool
is_equal (const Value &left, const Value &right) // NOLINT
{
  return std::visit (
      overloaded{ // Perform the C++ equality check if the two types match
                  is_equal_same_type{},
                  // Two non-mathching types can never be equal
                  [] (const auto &, const auto &) { return false; } },
      left, right);
}

Value
evaluate_plus_operator (const Value &left, const Token &op, const Value &right)
{
  return std::visit (
      overloaded{
          // numeric addition
          [] (const double &l, const double &r) -> Value { return l + r; },
          // string concatenation
          [] (const std::string &l, const std::string &r) -> Value {
            return l + r;
          },
          // anything else is an error
          [&op] (const auto &, const auto &) -> Value {
            throw RunTimeError (
                op, "Operands must be two numbers or two strings.");
          } },
      left, right);
}

void
check_number_operand (const Token &op, const Value &operand)
{
  if (!std::holds_alternative<double> (operand))
    throw RunTimeError (op, "Operand must be a number.");
}

void
check_number_operands (const Value &left, const Token &op, const Value &right)
{
  if (!(std::holds_alternative<double> (left)
        && std::holds_alternative<double> (right)))
    throw RunTimeError (op, "Operands must be numbers.");
}

std::string
stringify (const Value &value)
{
  return std::visit (
      overloaded{ [] (std::nullptr_t) -> std::string { return "nil"; },
                  [] (double d) -> std::string {
                    double i;
                    double fractional_part = std::modf (d, &i);
                    // TODO
                    if (fractional_part == 0.)
                      return std::to_string (i);
                    else
                      return std::to_string (d);
                  },
                  [] (bool b) -> std::string { return b ? "true" : "false"; },
                  [] (const std::string &s) { return s; } },
      value);
}

} // namespace

/**
 * A visitor to interpret a syntax tree.
 */
struct InterpreterVisitor
{
  /**
   * Helper to resolve the boxed content. Forwards the call to the unboxed
   * type T.
   */
  template <typename T>
  Value
  operator() (const Box<T> &boxed_expr) const
  {
    return this->operator() (*boxed_expr);
  }

  [[nodiscard]] Value visit (const Expr &expr) const;

  Value operator() (const ExprLiteral &expr) const;

  Value operator() (const ExprGrouping &expr) const;

  Value operator() (const ExprUnary &expr) const;

  Value operator() (const ExprBinary &expr) const;
};

Value
InterpreterVisitor::visit (const Expr &expr) const
{
  return std::visit (*this, expr);
}

Value
InterpreterVisitor::operator() (const ExprLiteral &expr) const
{
  return std::visit ([] (const auto &v) { return Value{ v }; }, expr.value);
}

Value
InterpreterVisitor::operator() (const ExprGrouping &expr) const
{
  return visit (expr.expression);
}

Value
InterpreterVisitor::operator() (const ExprUnary &expr) const
{
  Value right = visit (expr.right);
  switch (expr.op.type)
    {
    case TokenType::MINUS:
      check_number_operand (expr.op, right);
      return -numeric (right);
    case TokenType::BANG:
      return !is_truthy (right);
    default:
      return {};
    }
}

Value
InterpreterVisitor::operator() (const ExprBinary &expr) const
{
  const Value left = visit (expr.left);
  const Value right = visit (expr.right);
  switch (expr.op.type)
    {
    case TokenType::MINUS:
      check_number_operands (left, expr.op, right);
      return numeric (left) - numeric (right);
    case TokenType::SLASH:
      check_number_operands (left, expr.op, right);
      return numeric (left) / numeric (right);
    case TokenType::STAR:
      check_number_operands (left, expr.op, right);
      return numeric (left) * numeric (right);
    case TokenType::PLUS:
      return evaluate_plus_operator (left, expr.op, right);

    case TokenType::GREATER:
      check_number_operands (left, expr.op, right);
      return numeric (left) > numeric (right);
    case TokenType::GREATER_EQUAL:
      check_number_operands (left, expr.op, right);
      return numeric (left) >= numeric (right);
    case TokenType::LESS:
      check_number_operands (left, expr.op, right);
      return numeric (left) < numeric (right);
    case TokenType::LESS_EQUAL:
      check_number_operands (left, expr.op, right);
      return numeric (left) <= numeric (right);
    case TokenType::BANG_EQUAL:
      return !is_equal (left, right);
    case TokenType::EQUAL_EQUAL:
      return is_equal (left, right);
    default:
      return {};
    }
}

void
interpret (const Expr &expression)
{
  try
    {
      Value value = InterpreterVisitor{}.visit (expression);
      std::cout << stringify (value) << '\n';
    }
  catch (const RunTimeError &e)
    {
      run_time_error (e);
    }
}
} // namespace lox