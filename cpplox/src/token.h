#pragma once

#include <string>
#include <utility>
#include <variant>

namespace lox
{
enum class TokenType
{
  INVALID,
  // Single-character tokens.
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens.
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  TOK_EOF,
};

std::string to_string (const TokenType &t);

/**
 * Return the correct keyword token if the given @p text is indeed a keyword.
 * Otherwise, return INVALID.
 */
TokenType keyword (const std::string &text);

struct Token
{
  using Literal = std::variant<std::monostate, std::string, double>;

  Token (TokenType type, std::string lexeme, Literal literal, int line);

  [[nodiscard]] std::string to_string () const;

  //! Most important information: what type of token is this?
  const TokenType type;
  //! Only used for printing of this Token.
  const std::string lexeme;
  //! Only used if TokenType is a literal type
  const Literal literal;
  //! Only for error information
  const int line;
};

/**
 * String representation of the literal.
 */
std::string to_string (const Token::Literal &l);
} // namespace lox
