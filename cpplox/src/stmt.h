#pragma once

#include "expr.h"
#include <optional>
#include <variant>

namespace lox
{
struct StmtExpr;
struct StmtPrint;
struct StmtVar;

using Stmt = std::variant<StmtExpr, StmtPrint, StmtVar>;

struct StmtExpr
{
  Expr expression;
};

struct StmtPrint
{
  Expr expression;
};

struct StmtVar
{
  Token name;
  std::optional<Expr> initializer;
};
} // namespace lox
