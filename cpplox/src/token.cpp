#include "token.h"

#include <cassert>
#include <map>
#include <utility>
#include <variant>

namespace lox
{
namespace
{
const std::map<TokenType, std::string> token_to_string
    = { { TokenType::LEFT_PAREN, "LEFT_PAREN" },
        { TokenType::RIGHT_PAREN, "RIGHT_PAREN" },
        { TokenType::LEFT_BRACE, "LEFT_BRACE" },
        { TokenType::RIGHT_BRACE, "RIGHT_BRACE" },
        { TokenType::COMMA, "COMMA" },
        { TokenType::DOT, "DOT" },
        { TokenType::MINUS, "MINUS" },
        { TokenType::PLUS, "PLUS" },
        { TokenType::SEMICOLON, "SEMICOLON" },
        { TokenType::SLASH, "SLASH" },
        { TokenType::STAR, "STAR" },
        // One or two character tokens.
        { TokenType::BANG, "BANG" },
        { TokenType::BANG_EQUAL, "BANG_EQUAL" },
        { TokenType::EQUAL, "EQUAL" },
        { TokenType::EQUAL_EQUAL, "EQUAL_EQUAL" },
        { TokenType::GREATER, "GREATER" },
        { TokenType::GREATER_EQUAL, "GREATER_EQUAL" },
        { TokenType::LESS, "LESS" },
        { TokenType::LESS_EQUAL, "LESS_EQUAL" },
        // Literals.
        { TokenType::IDENTIFIER, "IDENTIFIER" },
        { TokenType::STRING, "STRING" },
        { TokenType::NUMBER, "NUMBER" },
        // Keywords.
        { TokenType::AND, "AND" },
        { TokenType::CLASS, "CLASS" },
        { TokenType::ELSE, "ELSE" },
        { TokenType::FALSE, "FALSE" },
        { TokenType::FUN, "FUN" },
        { TokenType::FOR, "FOR" },
        { TokenType::IF, "IF" },
        { TokenType::NIL, "NIL" },
        { TokenType::OR, "OR" },
        { TokenType::PRINT, "PRINT" },
        { TokenType::RETURN, "RETURN" },
        { TokenType::SUPER, "SUPER" },
        { TokenType::THIS, "THIS" },
        { TokenType::TRUE, "TRUE" },
        { TokenType::VAR, "VAR" },
        { TokenType::WHILE, "WHILE" },

        { TokenType::TOK_EOF, "TOK_EOF" } };

const std::map<std::string, TokenType> keywords = {
  { "and", TokenType::AND },       { "class", TokenType::CLASS },
  { "else", TokenType::ELSE },     { "false", TokenType::FALSE },
  { "fun", TokenType::FUN },       { "for", TokenType::FOR },
  { "if", TokenType::IF },         { "nil", TokenType::NIL },
  { "or", TokenType::OR },         { "print", TokenType::PRINT },
  { "return", TokenType::RETURN }, { "super", TokenType::SUPER },
  { "this", TokenType::THIS },     { "true", TokenType::TRUE },
  { "var", TokenType::VAR },       { "while", TokenType::WHILE },
};

} // namespace

std::string
to_string (const TokenType &t)
{
  assert (token_to_string.count (t) == 1);

  return token_to_string.at (t);
}

TokenType
keyword (const std::string &text)
{
  if (auto it = keywords.find (text); it != keywords.end ())
    return it->second;
  return TokenType::INVALID;
}

Token::Token (TokenType type, std::string lexeme, Literal literal, int line)
    : type (type), lexeme (std::move (lexeme)), literal (std::move (literal)),
      line (line)
{
}

std::string
Token::to_string () const
{
  // TODO see what we need here.
  return "{" + lox::to_string (type) + " " + lexeme + "}";
}

struct LiteralVisitor
{
  std::string
  operator() (const std::monostate &) const
  {
    return "nil";
  }
  std::string
  operator() (const std::string &s) const
  {
    return s;
  }
  std::string
  operator() (const double &d) const
  {
    return std::to_string (d);
  }
};

std::string
to_string (const Token::Literal &l)
{
  return std::visit (LiteralVisitor{}, l);
}
} // namespace lox
