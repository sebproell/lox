#include "parser.h"
#include "error.h"
#include "expr.h"
#include "token.h"

#include <cassert>
#include <memory>
#include <variant>
namespace lox
{

namespace internal
{
class ParserImpl
{
public:
  ParserImpl (std::vector<Token> &&tokens) : tokens (std::move (tokens)) {}

  std::vector<Stmt>
  parse ()
  {
    std::vector<Stmt> statements{};
    while (!is_at_end ())
      {
        // TODO restore error handling
        statements.emplace_back (declaration ());
      }
    return statements;
  }

private:
  /**
   * An exception type used to unwind parsing until a desired synchronization
   * point. Stack undwinding via exceptions is a natural choice to implement
   * synchronization.
   */
  class ParseError
  {
  };

  std::vector<Token> tokens;
  std::size_t current{};

  Stmt
  declaration ()
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
  var_declaration ()
  {
    Token name = consume (TokenType::IDENTIFIER, "Expect variable name.");

    std::optional<Expr> initializer{};
    if (match (TokenType::EQUAL))
      initializer = expression ();

    consume (TokenType::SEMICOLON, "Expect ';' after variable declaration.");

    return StmtVar{ name, initializer };
  }

  Stmt
  statement ()
  {
    if (match (TokenType::IF))
      return if_statement ();
    if (match (TokenType::PRINT))
      return print_statement ();
    if (match (TokenType::LEFT_BRACE))
      return block_statement ();
    else
      return expr_statement ();
  }

  Stmt
  if_statement ()
  {
    consume (TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    Expr condition = expression ();
    consume (TokenType::RIGHT_PAREN, "Expect ')' after 'if' condition.");

    Stmt then_branch = statement ();

    std::optional<Stmt> else_branch{};
    if (match (TokenType::ELSE))
      else_branch = statement ();

    return StmtIf{ condition, then_branch, else_branch };
  }

  Stmt
  print_statement ()
  {
    Expr value = expression ();
    consume (TokenType::SEMICOLON, "Expect ';' after value.");
    return StmtPrint{ std::move (value) };
  }

  Stmt
  block_statement ()
  {
    return StmtBlock{ until_end_of_block () };
  }

  std::vector<Stmt>
  until_end_of_block ()
  {
    std::vector<Stmt> statements;
    while (!check (TokenType::RIGHT_BRACE) && !is_at_end ())
      {
        statements.emplace_back (declaration ());
      }
    consume (TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
  }

  Stmt
  expr_statement ()
  {
    Expr value = expression ();
    consume (TokenType::SEMICOLON, "Expect ';' after value.");
    return StmtExpr{ std::move (value) };
  }

  Expr
  expression ()
  {
    return assignment ();
  }

  Expr
  assignment ()
  {
    // trick: parse the left-hand side as a value and figure out the target
    // afterwards
    Expr expr = equality ();

    if (match (TokenType::EQUAL))
      {
        Token equals = previous ();
        Expr rhs = assignment ();

        if (std::holds_alternative<ExprVariable> (expr))
          {
            const ExprVariable &expr_var = std::get<ExprVariable> (expr);
            return ExprAssign{ expr_var.name, rhs };
          }

        error (equals, "Invalid assignment target.");
      }

    // no assignment -> we already parsed the rest
    return expr;
  }

  Expr
  equality ()
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
  comparison ()
  {
    // comparison     → term ((">" | ">=" | "<" | "<=") term) *;
    Expr expr = term ();
    while (match (TokenType::GREATER, TokenType::GREATER_EQUAL,
                  TokenType::LESS, TokenType::LESS_EQUAL))
      {
        Token op = previous ();
        Expr right = term ();
        expr = ExprBinary{ expr, right, op };
      }
    return expr;
  }

  Expr
  term ()
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
  factor ()
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
  unary ()
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
  primary ()
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
  consume (TokenType type, const std::string &message)
  {
    if (check (type))
      return advance ();
    parse_error (peek (), message);
  }

  [[noreturn]] void
  parse_error (const Token &token, const std::string &message)
  {
    lox::error (peek (), message);
    throw ParseError{};
  }

  void
  synchronize ()
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
  advance ()
  {
    if (!is_at_end ())
      ++current;
    return previous ();
  }

  bool
  check (TokenType type)
  {
    if (is_at_end ())
      return false;
    return peek ().type == type;
  }

  bool
  is_at_end ()
  {
    return peek ().type == TokenType::TOK_EOF;
  }

  const Token &
  peek ()
  {
    return tokens[current];
  }

  const Token &
  previous ()
  {
    assert (current > 0);
    return tokens[current - 1];
  }

  // Match and advance.
  template <typename... TokenT>
  bool
  match (TokenT... ts)
  {
    const bool any_match = (... || check (ts));
    if (any_match)
      advance ();
    return any_match;
  }
};

// --- template and inline functions --- //

} // namespace internal

Parser::Parser (std::vector<Token> tokens)
    : pimpl (std::make_unique<internal::ParserImpl> (std::move (tokens)))
{
}

//! Default in implementation to allow PIMPL with unique_ptr
Parser::~Parser () = default;

std::vector<Stmt>
Parser::parse ()
{
  return pimpl->parse ();
}

} // namespace lox
