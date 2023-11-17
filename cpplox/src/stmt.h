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
struct StmtIf;
struct StmtWhile;
struct StmtFunction;
struct StmtReturn;

using Stmt = std::variant<StmtExpr, StmtPrint, StmtVar, StmtBlock, Box<StmtIf>,
                          Box<StmtWhile>, Box<StmtFunction>, StmtReturn>;

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

struct StmtReturn
{
  Token keyword;
  std::optional<Expr> value;
};

struct StmtBlock
{
  std::vector<Stmt> statements;
};

struct StmtIf
{
  Expr condition;
  Stmt then_branch;
  std::optional<Stmt> else_branch;
};

struct StmtWhile
{
  Expr condition;
  Stmt body;
};

struct StmtFunction
{
  Token name;
  std::vector<Token> params;
  std::vector<Stmt> body;
};

} // namespace lox
