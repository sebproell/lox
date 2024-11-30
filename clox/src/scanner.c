#include "scanner.h"
#include <stdbool.h>
#include <string.h>

typedef struct
{
  const char *start;
  const char *current;
  int line;
} Scanner;

Scanner scanner;

void
init_scanner (const char *source)
{
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

static bool
is_at_end ()
{
  return *scanner.current == '\0';
}

static Token
make_token (TokenType type)
{
  Token t = { .type = type,
              .start = scanner.start,
              .length = scanner.current - scanner.start,
              .line = scanner.line };
  return t;
}

static Token
error_token (const char *err_msg)
{
  Token err = { .type = TOKEN_ERROR,
                .start = err_msg,
                .length = (int)strlen (err_msg),
                .line = scanner.line };
  return err;
}

static char
advance ()
{
  return *(scanner.current++);
}

static bool
match (char expected)
{
  if (is_at_end ())
    return false;

  if (*scanner.current != expected)
    return false;

  scanner.current++;
  return true;
}

static char
peek ()
{
  return *scanner.current;
}

static char
peek_next ()
{
  if (is_at_end ())
    return '\0';
  return *(scanner.current + 1);
}

static void
skip_whitespace ()
{
  for (;;)
    {
      char c = peek ();
      switch (c)
        {
        case ' ':
        case '\t':
        case '\r':
          advance ();
          break;
        case '\n':
          ++scanner.line;
          advance ();
          break;
        case '/':
          // treat comments as whitespace
          if (peek_next () == '/')
            {
              // eat the rest of the line
              while (peek () != '\n' && !is_at_end ())
                advance ();
            }
          else
            return;
          break;
        default:
          return;
        }
    }
}

static Token
string ()
{
  while (peek () != '"' && !is_at_end ())
    {
      if (peek () == '\n')
        ++scanner.line;
      advance ();
    }

  if (is_at_end ())
    return error_token ("Unterminated string.");
  // Closing quote.
  advance ();
  return make_token (TOKEN_STRING);
}

static bool
is_digit (char c)
{
  return c >= '0' && c <= '9';
}

static bool
is_alpha (char c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static Token
number ()
{
  // Mandatory part in front of decimal
  while (is_digit (peek ()))
    advance ();

  // Optional decimal followed by more digits
  if (peek () == '.' && is_digit (peek_next ()))
    {
      advance ();
      while (is_digit (peek ()))
        advance ();
    }

  return make_token (TOKEN_NUMBER);
}

static TokenType
check_keyword (int start, int length, const char *rest, TokenType type)
{
  if (scanner.current - scanner.start == start + length
      && memcmp (scanner.start + start, rest, length) == 0)
    return type;

  return TOKEN_IDENTIFIER;
}

// Called when the scanner has fully parsed an identifier and we just need to
// figure out its type.
static TokenType
identifier_type ()
{
  switch (scanner.start[0])
    {
    case 'a':
      return check_keyword (1, 2, "nd", TOKEN_AND);
    case 'c':
      return check_keyword (1, 4, "lass", TOKEN_CLASS);
    case 'e':
      return check_keyword (1, 3, "lse", TOKEN_ELSE);
    case 'f':
      if (scanner.current - scanner.start > 1)
        {
          switch (scanner.start[1])
            {
            case 'a':
              return check_keyword (2, 3, "lse", TOKEN_FALSE);
            case 'o':
              return check_keyword (2, 1, "r", TOKEN_FOR);
            case 'u':
              return check_keyword (2, 1, "n", TOKEN_FUN);
            }
        }
      break;
    case 'i':
      return check_keyword (1, 1, "f", TOKEN_IF);
    case 'n':
      return check_keyword (1, 2, "il", TOKEN_NIL);
    case 'o':
      return check_keyword (1, 1, "r", TOKEN_OR);
    case 'p':
      return check_keyword (1, 4, "rint", TOKEN_PRINT);
    case 'r':
      return check_keyword (1, 5, "eturn", TOKEN_RETURN);
    case 's':
      return check_keyword (1, 4, "uper", TOKEN_SUPER);
    case 't':
      if (scanner.current - scanner.start > 1)
        {
          switch (scanner.start[1])
            {
            case 'h':
              return check_keyword (2, 2, "is", TOKEN_THIS);
            case 'r':
              return check_keyword (2, 2, "ue", TOKEN_TRUE);
            }
        }
      break;
    case 'v':
      return check_keyword (1, 2, "ar", TOKEN_VAR);
    case 'w':
      return check_keyword (1, 4, "hile", TOKEN_WHILE);
    }
  return TOKEN_IDENTIFIER;
}

static Token
identifier ()
{
  while (is_alpha (peek ()) || is_digit (peek ()))
    advance ();
  return make_token (identifier_type ());
}

Token
scan_token ()
{
  skip_whitespace ();
  scanner.start = scanner.current;

  if (is_at_end ())
    return make_token (TOKEN_EOF);

  char c = advance ();

  if (is_alpha (c))
    return identifier ();
  if (is_digit (c))
    return number ();

#define MAKE_TOKEN_WITH_EQUALS_IF_MATCH(base_name)                            \
  make_token (match ('=') ? base_name##_EQUAL : base_name)

  switch (c)
    {
    case '(':
      return make_token (TOKEN_LEFT_PAREN);
    case ')':
      return make_token (TOKEN_RIGHT_PAREN);
    case '{':
      return make_token (TOKEN_LEFT_BRACE);
    case '}':
      return make_token (TOKEN_RIGHT_BRACE);
    case ';':
      return make_token (TOKEN_SEMICOLON);
    case ',':
      return make_token (TOKEN_COMMA);
    case '.':
      return make_token (TOKEN_DOT);
    case '-':
      return make_token (TOKEN_MINUS);
    case '+':
      return make_token (TOKEN_PLUS);
    case '/':
      return make_token (TOKEN_SLASH);
    case '*':
      return make_token (TOKEN_STAR);
    case '!':
      return MAKE_TOKEN_WITH_EQUALS_IF_MATCH (TOKEN_BANG);
    case '=':
      return MAKE_TOKEN_WITH_EQUALS_IF_MATCH (TOKEN_EQUAL);
    case '<':
      return MAKE_TOKEN_WITH_EQUALS_IF_MATCH (TOKEN_LESS);
    case '>':
      return MAKE_TOKEN_WITH_EQUALS_IF_MATCH (TOKEN_GREATER);
    case '"':
      return string ();
    }

#undef MAKE_TOKEN_WITH_EQUALS_IF_MATCH

  return error_token ("Unexpected character.");
}

const char *
token_type_to_string (TokenType type)
{
  switch (type)
    {
    case TOKEN_LEFT_PAREN:
      return "TOKEN_LEFT_PAREN";
    case TOKEN_RIGHT_PAREN:
      return "TOKEN_RIGHT_PAREN";
    case TOKEN_LEFT_BRACE:
      return "TOKEN_LEFT_BRACE";
    case TOKEN_RIGHT_BRACE:
      return "TOKEN_RIGHT_BRACE";
    case TOKEN_COMMA:
      return "TOKEN_COMMA";
    case TOKEN_DOT:
      return "TOKEN_DOT";
    case TOKEN_MINUS:
      return "TOKEN_MINUS";
    case TOKEN_PLUS:
      return "TOKEN_PLUS";
    case TOKEN_SEMICOLON:
      return "TOKEN_SEMICOLON";
    case TOKEN_SLASH:
      return "TOKEN_SLASH";
    case TOKEN_STAR:
      return "TOKEN_STAR";
    case TOKEN_BANG:
      return "TOKEN_BANG";
    case TOKEN_BANG_EQUAL:
      return "TOKEN_BANG_EQUAL";
    case TOKEN_EQUAL:
      return "TOKEN_EQUAL";
    case TOKEN_EQUAL_EQUAL:
      return "TOKEN_EQUAL_EQUAL";
    case TOKEN_GREATER:
      return "TOKEN_GREATER";
    case TOKEN_GREATER_EQUAL:
      return "TOKEN_GREATER_EQUAL";
    case TOKEN_LESS:
      return "TOKEN_LESS";
    case TOKEN_LESS_EQUAL:
      return "TOKEN_LESS_EQUAL";
    case TOKEN_IDENTIFIER:
      return "TOKEN_IDENTIFIER";
    case TOKEN_STRING:
      return "TOKEN_STRING";
    case TOKEN_NUMBER:
      return "TOKEN_NUMBER";
    case TOKEN_AND:
      return "TOKEN_AND";
    case TOKEN_CLASS:
      return "TOKEN_CLASS";
    case TOKEN_ELSE:
      return "TOKEN_ELSE";
    case TOKEN_FALSE:
      return "TOKEN_FALSE";
    case TOKEN_FOR:
      return "TOKEN_FOR";
    case TOKEN_FUN:
      return "TOKEN_FUN";
    case TOKEN_IF:
      return "TOKEN_IF";
    case TOKEN_NIL:
      return "TOKEN_NIL";
    case TOKEN_OR:
      return "TOKEN_OR";
    case TOKEN_PRINT:
      return "TOKEN_PRINT";
    case TOKEN_RETURN:
      return "TOKEN_RETURN";
    case TOKEN_SUPER:
      return "TOKEN_SUPER";
    case TOKEN_THIS:
      return "TOKEN_THIS";
    case TOKEN_TRUE:
      return "TOKEN_TRUE";
    case TOKEN_VAR:
      return "TOKEN_VAR";
    case TOKEN_WHILE:
      return "TOKEN_WHILE";
    case TOKEN_ERROR:
      return "TOKEN_ERROR";
    case TOKEN_EOF:
      return "TOKEN_EOF";
    }
}
