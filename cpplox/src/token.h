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

using Literal = std::variant<std::monostate, std::string, double>;

struct Token
{
  Token (TokenType type, std::string lexeme, Literal literal, int line);

  [[nodiscard]] std::string to_string () const;

  const TokenType type;
  const std::string lexeme;
  const Literal literal;
  const int line;
};
}