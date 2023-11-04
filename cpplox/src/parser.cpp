#include "parser.h"
#include "error.h"
#include "token.h"

#include <cassert>
namespace lox
{
Parser::Parser (std::vector<Token> tokens) : tokens (std::move (tokens)) {}

std::vector<Stmt>
Parser::parse ()
{
  std::vector<Stmt> statements{};
  while (!is_at_end ())
    {
      // TODO restore error handling
      statements.emplace_back (declaration ());
    }
  return statements;
}

Stmt
Parser::declaration ()
{
  try
    {
      if (match (TokenType::VAR))
        return var_declaration ();
      return statement ();
    }
  catch (const ParseError &error)
    {
      synchronize ();
      // Return an arbitrary Statement, we will never evaluate it.
      return {};
    }
}

Stmt
Parser::var_declaration ()
{
  Token name = consume (TokenType::IDENTIFIER, "Expect variable name.");

  std::optional<Expr> initializer{};
  if (match (TokenType::EQUAL))
    initializer = expression ();

  consume (TokenType::SEMICOLON, "Expect ';' after variable declaration.");

  return StmtVar{ name, initializer };
}

Stmt
Parser::statement ()
{
  if (match (TokenType::PRINT))
    return print_statement ();
  else
    return expr_statement ();
}

Stmt
Parser::print_statement ()
{
  Expr value = expression ();
  consume (TokenType::SEMICOLON, "Expect ';' after value.");
  return StmtPrint{ std::move (value) };
}

Stmt
Parser::expr_statement ()
{
  Expr value = expression ();
  consume (TokenType::SEMICOLON, "Expect ';' after value.");
  return StmtExpr{ std::move (value) };
}

Expr
Parser::expression ()
{
  // expression     → equality;
  return equality ();
}

Expr
Parser::equality ()
{
  // equality       → comparison (("!=" | "==") comparison) *;
  Expr expr = comparison ();
  while (match (TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
    {
      Token op = previous ();
      Expr right = comparison ();
      expr = ExprBinary{ expr, right, op };
    }
  return expr;
}

Expr
Parser::comparison ()
{
  // comparison     → term ((">" | ">=" | "<" | "<=") term) *;
  Expr expr = term ();
  while (match (TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                TokenType::LESS_EQUAL))
    {
      Token op = previous ();
      Expr right = term ();
      expr = ExprBinary{ expr, right, op };
    }
  return expr;
}

Expr
Parser::term ()
{
  // term           → factor (("-" | "+") factor) *;
  Expr expr = factor ();
  while (match (TokenType::MINUS, TokenType::PLUS))
    {
      Token op = previous ();
      Expr right = factor ();
      expr = ExprBinary{ expr, right, op };
    }
  return expr;
}

Expr
Parser::factor ()
{
  // factor         → unary (("/" | "*") unary) *;
  Expr expr = unary ();
  while (match (TokenType::SLASH, TokenType::STAR))
    {
      Token op = previous ();
      Expr right = unary ();
      expr = ExprBinary{ expr, right, op };
    }
  return expr;
}

Expr
Parser::unary ()
{
  // unary          → ("!" | "-") unary | primary;
  if (match (TokenType::BANG, TokenType::MINUS))
    {
      Token op = previous ();
      Expr right = unary ();
      return ExprUnary{ right, op };
    }
  return primary ();
}

Expr
Parser::primary ()
{
  // primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expr ")";
  if (match (TokenType::FALSE))
    return ExprLiteral{ false };
  if (match (TokenType::TRUE))
    return ExprLiteral{ true };
  if (match (TokenType::NIL))
    return ExprLiteral{ nullptr };

  if (match (TokenType::NUMBER, TokenType::STRING))
    {
      const auto &literal = previous ().literal;
      assert (literal.has_value ());
      return ExprLiteral{ *literal };
    }

  if (match (TokenType::IDENTIFIER))
    return ExprVariable{ previous () };

  if (match (TokenType::LEFT_PAREN))
    {
      Expr expr = expression ();
      consume (TokenType::RIGHT_PAREN, "Expect ')' after expression.");
      return ExprGrouping{ expr };
    }

  parse_error (peek (), "Expect expression.");
}

const Token &
Parser::consume (TokenType type, const std::string &message)
{
  if (check (type))
    return advance ();
  parse_error (peek (), message);
}

void
Parser::parse_error (const Token &token, const std::string &message)
{
  lox::error (peek (), message);
  throw ParseError{};
}

void
Parser::synchronize ()
{
  advance ();
  while (!is_at_end ())
    {
      // We stepped over a semicolon which must mark the end of a statement
      if (previous ().type == TokenType::SEMICOLON)
        return;

      // Peek whether bnex token is beginnign of a statement.
      switch (peek ().type)
        {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
          return;
        default:
          continue;
        }
    }
}

const Token &
Parser::advance ()
{
  if (!is_at_end ())
    ++current;
  return previous ();
}

bool
Parser::check (TokenType type)
{
  if (is_at_end ())
    return false;
  return peek ().type == type;
}

bool
Parser::is_at_end ()
{
  return peek ().type == TokenType::TOK_EOF;
}

const Token &
Parser::peek ()
{
  return tokens[current];
}

const Token &
Parser::previous ()
{
  assert (current > 0);
  return tokens[current - 1];
}
} // namespace lox
