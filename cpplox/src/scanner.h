#pragma once

#include "error.h"
#include "token.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace lox
{
class Scanner
{
public:
  Scanner (std::string source) : source (std::move (source)) {}

  std::vector<Token>
  scan_tokens ()
  {
    while (!at_end ())
      {
        start = current;
        scan_token ();
      }
    tokens.emplace_back (TokenType::TOK_EOF, "", Token::Literal{}, line);
    return tokens;
  }

private:
  void
  scan_token ()
  {
    char c = advance ();
    switch (c)
      {
      case '(':
        add_token (TokenType::LEFT_PAREN);
        break;
      case ')':
        add_token (TokenType::RIGHT_PAREN);
        break;
      case '{':
        add_token (TokenType::LEFT_BRACE);
        break;
      case '}':
        add_token (TokenType::RIGHT_BRACE);
        break;
      case ',':
        add_token (TokenType::COMMA);
        break;
      case '.':
        add_token (TokenType::DOT);
        break;
      case '-':
        add_token (TokenType::MINUS);
        break;
      case '+':
        add_token (TokenType::PLUS);
        break;
      case ';':
        add_token (TokenType::SEMICOLON);
        break;
      case '*':
        add_token (TokenType::STAR);
        break;

      case '!':
        add_token (match ('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
      case '=':
        add_token (match ('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
      case '<':
        add_token (match ('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
      case '>':
        add_token (match ('=') ? TokenType::GREATER_EQUAL
                               : TokenType::GREATER);
        break;

      case '/':
        if (match ('/'))
          {
            // absorb all character following a comment until the end of the
            // line
            while (peek () != '\n' && !at_end ())
              advance ();
          }
        else
          add_token (TokenType::SLASH);
        break;

      // Ignored whitespace
      case ' ':
      case '\r':
      case '\t':
        break;

      case '\n':
        line++;
        break;

      case '"':
        parse_string ();
        break;

      default:
        if (std::isdigit (c))
          parse_number ();
        else if (std::isalpha (c))
          parse_identifier ();
        else
          error (line, "Unrecognized character.");
        break;
      }
  }

  void
  parse_string ()
  {
    // Advance until the next character is an ".
    while (peek () != '"' && !at_end ())
      {
        if (peek () == '\n')
          ++line;
        advance ();
      }

    if (at_end ())
      {
        error (line, "Unterminated string");
        return;
      }

    // Advance over the closing ".
    advance ();

    // Discard the quotes by shifitng 1 character.
    add_token (TokenType::STRING,
               source.substr (start + 1, current - start - 1));
  }

  void
  parse_number ()
  {
    while (std::isdigit (peek ()))
      advance ();

    if (peek () == '.' && std::isdigit (peek_next ()))
      {
        // Consume the decimal point
        advance ();

        while (std::isdigit (peek ()))
          advance ();
      }

    add_token (TokenType::NUMBER,
               std::stod (source.substr (start, current - start)));
  }

  void
  parse_identifier ()
  {
    const auto is_alpha_numeric
        = [] (char c) { return std::isdigit (c) || std::isalpha (c); };

    while (is_alpha_numeric (peek ()))
      advance ();

    TokenType type = keyword (source.substr (start, current - start));
    if (type == TokenType::INVALID)
      type = TokenType::IDENTIFIER;

    add_token (type);
  }

  void
  add_token (TokenType t)
  {
    add_token (t, Token::Literal{});
  }

  void
  add_token (TokenType t, Token::Literal literal)
  {
    tokens.emplace_back (t, source.substr (start, current - start), literal,
                         line);
  }

  char
  advance ()
  {
    assert (!at_end ());
    return source.at (current++);
  }

  [[nodiscard]] bool
  at_end () const
  {
    return current >= source.length ();
  }

  bool
  match (char expected)
  {
    if (at_end ())
      return false;
    if (source.at (current) != expected)
      return false;

    current++;
    return true;
  }

  [[nodiscard]] char
  peek () const
  {
    if (at_end ())
      return '\0';
    return source.at (current);
  }

  [[nodiscard]] char
  peek_next () const
  {
    if (current + 1 >= source.length ())
      return '\0';
    return source.at (current + 1);
  }

  std::string source;

  //! Beginning of lexeme.
  int start{};
  //! Offset from #start within the current lexeme.
  int current{};

  //! Line information purely for error and debugging purposes
  int line{ 1 };

  std::vector<Token> tokens;
};
} // namespace lox
