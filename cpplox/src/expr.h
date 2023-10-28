#pragma once

#include "box.h"
#include "token.h"

#include <memory>
#include <variant>

namespace lox
{

struct ExprBinary;
struct ExprGrouping;
struct ExprLiteral;
struct ExprUnary;

/**
 * Expressions are implemented with the Visitor design pattern. In C++, this is
 * best done with a variant type. To enable recursion within the types included
 * in the variant, the recursive types are put into the Box helper type which
 * heap allocates memory.
 */
using Expr = std::variant<ExprLiteral, Box<ExprBinary>, Box<ExprUnary>,
                          Box<ExprGrouping> >;

struct ExprLiteral
{
  Token::Literal value;
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

struct ExprGrouping
{
  Expr expression;
};

} // namespace lox
