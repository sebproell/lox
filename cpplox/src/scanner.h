#pragma once

#include "error.h"
#include "token.h"

#include <cassert>
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
    tokens.emplace_back (TokenType::TOK_EOF, "", Literal{}, line);
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
        error (line, "Unrecognized character.");
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

    add_token (TokenType::STRING, source.substr (start + 1, current - 1));
  }

  void
  add_token (TokenType t)
  {
    add_token (t, Literal{});
  }

  void
  add_token (TokenType t, Literal literal)
  {
    tokens.emplace_back (t, source.substr (start, current), literal, line);
  }

  char
  advance ()
  {
    assert (!at_end ());
    return source.at (current++);
  }

  bool
  at_end ()
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

  char
  peek ()
  {
    if (at_end ())
      return '\0';
    return source.at (current);
  }

  std::string source;

  //! Beginning of lexeme.
  int start{};
  //! Offset frmo #start within the current lexeme.
  int current{};

  //! Line information purely for error and debugging purposes
  int line{ 1 };

  std::vector<Token> tokens;
};
}
