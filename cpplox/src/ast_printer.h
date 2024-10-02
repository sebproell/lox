#pragma once

#include "expr.h"
#include "stmt.h"
#include <sstream>
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

  template <typename T>
  [[nodiscard]] std::string
  visit (const std::vector<T> &list) const
  {
    std::stringstream ss;
    for (const auto &entry : list)
      {
        ss << visit (entry);
      }
    return ss.str ();
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
  operator() (const StmtVar &stmt) const
  {
    return "(var " + stmt.name.lexeme
           + (stmt.initializer ? (" = " + visit (*stmt.initializer)) : "")
           + ")";
  }

  std::string
  operator() (const StmtReturn &stmt) const
  {
    return "(return " + (stmt.value ? visit (*stmt.value) : "") + ")";
  }

  std::string
  operator() (const StmtBlock &stmt) const
  {
    std::stringstream ss;
    ss << "(block ";
    ss << visit (stmt.statements);
    ss << ")";
    return ss.str ();
  }

  std::string
  operator() (const StmtIf &stmt) const
  {
    std::stringstream ss;
    ss << "(if ";
    ss << visit (stmt.condition);
    ss << visit (stmt.then_branch);

    if (stmt.else_branch)
      ss << " else " << visit (*stmt.else_branch);

    ss << ")";
    return ss.str ();
  }

  std::string
  operator() (const StmtWhile &stmt) const
  {
    std::stringstream ss;
    ss << "(if ";
    ss << visit (stmt.condition);
    ss << visit (stmt.body);
    ss << ")";
    return ss.str ();
  }

  std::string
  operator() (const StmtFunction &stmt) const
  {
    std::stringstream ss;
    ss << "(fun " << (stmt.name.lexeme);
    ss << "(";
    for (const auto &t : stmt.params)
      ss << t.lexeme;
    ss << ")";
    ss << "{" << visit (stmt.body) << "}";
    ss << ")";
    return ss.str ();
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
    return binary (expr);
  }

  std::string
  operator() (const ExprVariable &expr) const
  {
    return "(var " + expr.name.lexeme + ")";
  }

  std::string
  operator() (const ExprAssign &expr) const
  {
    return "(assign " + expr.name.lexeme + visit (expr.value) + ")";
  }

  std::string
  operator() (const ExprCall &expr) const
  {
    return "(call " + visit (expr.callee) + "(" + visit (expr.arguments)
           + "))";
  }

  std::string
  operator() (const ExprLogical &expr) const
  {
    return binary (expr);
  }

  template <typename BinaryExprType>
  [[nodiscard]] std::string
  binary (const BinaryExprType &expr) const
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
