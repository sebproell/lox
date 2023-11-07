#include "interpreter.h"
#include "environment.h"
#include "error.h"
#include "expr.h"
#include "stmt.h"
#include "token.h"
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <variant>

namespace lox
{

/**
 * Various helpers when interpreting an AST.
 */
namespace
{
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
                  // Two non-matching types can never be equal
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
                    std::string s = std::to_string (d);
                    if (fractional_part == 0.)
                      return s.substr (0, s.find ("."));
                    else
                      return s;
                  },
                  [] (bool b) -> std::string { return b ? "true" : "false"; },
                  [] (const std::string &s) { return s; } },
      value);
}

} // namespace

/**
 * A visitor to interpret a syntax tree.
 *
 * This class is a visior for both Expr and Stmt variants.
 */
struct InterpreterVisitor
{

  /**
   * The visitor may require the environment of variables.
   * This is a mutable pointer to allow changing the environment in different
   * scopes.
   */
  mutable Environment *env;

  /**
   * Helper to resolve the boxed content. Forwards the call to the unboxed
   * type T. Notaby, this works for boxed Stmt _and_ Expr variants.
   */
  template <typename T>
  auto
  operator() (const Box<T> &boxed_expr) const
  {
    return this->operator() (*boxed_expr);
  }

  [[nodiscard]] Value
  evaluate (const Expr &expr) const
  {
    return std::visit (*this, expr);
  }

  void
  execute (const Stmt &stmt) const
  {
    std::visit (*this, stmt);
  }

  void
  operator() (const StmtPrint &stmt) const
  {
    Value val = evaluate (stmt.expression);
    std::cout << stringify (val) << '\n';
  }

  void
  operator() (const StmtExpr &stmt) const
  {
    // Evaluate an expression for side effects and discard the result
    (void)evaluate (stmt.expression);
  }

  void
  operator() (const StmtVar &stmt) const
  {
    Value value = nullptr;
    if (stmt.initializer)
      value = evaluate (*stmt.initializer);

    env->define (stmt.name.lexeme, value);
  }

  void
  operator() (const StmtBlock &stmt) const
  {
    execute_block (stmt.statements);
  }

  void
  operator() (const StmtIf &stmt) const
  {
    if (is_truthy (evaluate (stmt.condition)))
      execute (stmt.then_branch);
    else if (stmt.else_branch)
      execute (*stmt.else_branch);
  }

  void
  operator() (const StmtWhile &stmt) const
  {
    while (is_truthy (evaluate (stmt.condition)))
      execute (stmt.body);
  }

  void
  execute_block (const std::vector<Stmt> &stmts) const
  {
    // Set up a block environment and use that as the inner-most environment
    // during evaluation of the block's statements. Make sure to restore the
    // previous environment on all(!) exit paths via RAII.
    const auto delete_block_env = [this, previous = env] (auto *block_env) {
      delete block_env;
      // restore the old environment
      this->env = previous;
    };
    std::unique_ptr<Environment, decltype (delete_block_env)> block_env (
        new Environment (env), delete_block_env);
    this->env = block_env.get ();

    for (const auto &stmt : stmts)
      execute (stmt);
  }

  [[nodiscard]] Value
  operator() (const ExprLiteral &expr) const
  {
    return std::visit ([] (const auto &v) { return Value{ v }; }, expr.value);
  }

  [[nodiscard]] Value
  operator() (const ExprGrouping &expr) const
  {
    return evaluate (expr.expression);
  }

  [[nodiscard]] Value
  operator() (const ExprUnary &expr) const
  {
    Value right = evaluate (expr.right);
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

  [[nodiscard]] Value
  operator() (const ExprBinary &expr) const
  {
    const Value left = evaluate (expr.left);
    const Value right = evaluate (expr.right);
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

  [[nodiscard]] Value
  operator() (const ExprVariable &expr) const
  {
    assert (env != nullptr);
    return (*env)[expr.name];
  }

  [[nodiscard]] Value
  operator() (const ExprAssign &expr) const
  {
    assert (env != nullptr);
    Value value = evaluate (expr.value);
    (*env)[expr.name] = value;
    return value;
  }

  [[nodiscard]] Value
  operator() (const ExprLogical &expr) const
  {
    Value left = evaluate (expr.left);

    if (expr.op.type == TokenType::OR && is_truthy (left))
      return left;
    if (expr.op.type == TokenType::AND && !is_truthy (left))
      return left;

    return evaluate (expr.right);
  }
};

namespace internal
{
class InterpreterImpl
{
public:
  Environment env;
};
} // namespace internal

Interpreter::Interpreter ()
    : pimpl (std::make_unique<internal::InterpreterImpl> ())
{
}

// Default the destructor here to use unique_ptr as pimpl.
Interpreter::~Interpreter () = default;

void
Interpreter::interpret (const std::vector<Stmt> &program)
{
  try
    {
      for (const auto &stmt : program)
        interpret (stmt);
    }
  catch (const RunTimeError &e)
    {
      run_time_error (e);
    }
}

void
Interpreter::interpret (const Stmt &stmt)
{
  try
    {
      InterpreterVisitor{ &pimpl->env }.execute (stmt);
    }
  catch (const RunTimeError &e)
    {
      run_time_error (e);
    }
}
} // namespace lox
