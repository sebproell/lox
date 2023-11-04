#pragma once

#include "expr.h"
#include "stmt.h"
#include <variant>

namespace lox
{
struct AstPrinterVisitor
{
  /**
   * Helper to resolve the boxed content. Forwards the call to the unboxed
   * type T.
   */
  template <typename T>
  std::string
  operator() (const Box<T> &boxed_expr) const
  {
    return this->operator() (*boxed_expr);
  }

  [[nodiscard]] std::string
  visit (const Expr &expr) const
  {
    return std::visit (*this, expr);
  }

  [[nodiscard]] std::string
  visit (const Stmt &stmt) const
  {
    return std::visit (*this, stmt);
  }

  std::string
  operator() (const StmtPrint &stmt) const
  {
    return "(print " + visit (stmt.expression) + ")";
  }

  std::string
  operator() (const StmtExpr &stmt) const
  {
    return "(expr " + visit (stmt.expression) + ")";
  }

  std::string
  operator() (const ExprLiteral &expr) const
  {
    return to_string (expr.value);
  }

  std::string
  operator() (const ExprGrouping &expr) const
  {
    return "(group " + visit (expr.expression) + ")";
  }

  std::string
  operator() (const ExprUnary &expr) const
  {
    return "(" + expr.op.lexeme + " " + visit (expr.right) + ")";
  }

  std::string
  operator() (const ExprBinary &expr) const
  {
    return "(" + visit (expr.left) + " " + expr.op.lexeme + " "
           + visit (expr.right) + ")";
  }
};

inline std::string
print_ast (const Expr &expression)
{
  return AstPrinterVisitor{}.visit (expression);
}

inline std::string
print_ast (const Stmt &stmt)
{
  return AstPrinterVisitor{}.visit (stmt);
}
} // namespace lox
