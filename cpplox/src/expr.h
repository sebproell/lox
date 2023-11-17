#pragma once

#include "box.h"
#include "token.h"

#include <memory>
#include <variant>
#include <vector>

namespace lox
{

struct ExprBinary;
struct ExprGrouping;
struct ExprLiteral;
struct ExprLogical;
struct ExprUnary;
struct ExprVariable;
struct ExprAssign;
struct ExprCall;

/**
 * Expressions are implemented with the Visitor design pattern. In C++, this is
 * best done with a variant type. To enable recursion within the types included
 * in the variant, the recursive types are put into the Box helper type which
 * heap allocates memory.
 */
using Expr = std::variant<ExprLiteral, ExprVariable, Box<ExprLogical>,
                          Box<ExprBinary>, Box<ExprUnary>, Box<ExprGrouping>,
                          Box<ExprAssign>, Box<ExprCall> >;

struct ExprLiteral
{
  Literal value;
};

struct ExprVariable
{
  Token name;
};

struct ExprLogical
{
  Expr left;
  Expr right;
  Token op;
};

struct ExprUnary
{
  Expr right;
  Token op;
};

struct ExprBinary
{
  Expr left;
  Expr right;
  Token op;
};

struct ExprAssign
{
  Token name;
  Expr value;
};

struct ExprGrouping
{
  Expr expression;
};

struct ExprCall
{
  Expr callee;
  Token paren; // for runtime error reporting
  std::vector<Expr> arguments;
};

} // namespace lox
