#pragma once

#include "expr.h"
#include <variant>

namespace lox
{
struct StmtExpr;
struct StmtPrint;

using Stmt = std::variant<StmtExpr, StmtPrint>;

struct StmtExpr
{
  Expr expression;
};

struct StmtPrint
{
  Expr expression;
};

} // namespace lox
