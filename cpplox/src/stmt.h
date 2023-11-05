#pragma once

#include "expr.h"
#include <optional>
#include <variant>
#include <vector>

namespace lox
{
struct StmtExpr;
struct StmtPrint;
struct StmtVar;
struct StmtBlock;

using Stmt = std::variant<StmtExpr, StmtPrint, StmtVar, StmtBlock>;

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

struct StmtBlock
{
  std::vector<Stmt> statements;
};
} // namespace lox
