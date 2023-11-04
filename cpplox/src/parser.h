#pragma once

#include "expr.h"
#include "stmt.h"
#include "token.h"

#include <vector>

namespace lox
{
class Parser
{
public:
  Parser (std::vector<Token> tokens);

  /**
   * Parse the sequence of Tokens in this Parser into a program, i.e., a series
   * of statements.
   */
  std::vector<Stmt> parse ();

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

  Stmt statement ();

  Stmt print_statement ();

  Stmt expr_statement ();

  Expr expression ();

  Expr equality ();

  Expr comparison ();

  Expr term ();

  Expr factor ();

  Expr unary ();

  Expr primary ();

  // Match the token and advance OR throw an error and initiate
  // synchronization.
  // @return The consumed Token.
  const Token &consume (TokenType type, const std::string &message);

  [[noreturn]] void parse_error (const Token &token,
                                 const std::string &message);

  // Discard tokens until finding the beginning of a statement.
  void synchronize ();

  // Match and advance.
  template <typename... TokenT> bool match (TokenT... ts);

  // Advance to next token and return the previous token.
  const Token &advance ();

  // Check but don't advance.
  bool check (TokenType type);

  bool is_at_end ();

  const Token &peek ();

  const Token &previous ();
};

// --- template and inline functions --- //

template <typename... TokenT>
bool
Parser::match (TokenT... ts)
{
  const bool any_match = (... || check (ts));
  if (any_match)
    advance ();
  return any_match;
}
} // namespace lox
