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
 * best done with a variant type. A std::monostate is also included to allow a
 * default constructed (but invalid) expression.
 */
using Expr = std::variant<std::monostate, ExprLiteral, Box<ExprBinary>,
                          Box<ExprUnary>, Box<ExprGrouping> >;

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
