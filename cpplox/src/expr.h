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
