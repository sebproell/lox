#include "error.h"
#include "token.h"
#include <iostream>

namespace lox
{

bool had_error = false;
bool had_run_time_error = false;

void
error (int line, const std::string &message)
{
  report (line, "", message);
}

void
error (Token token, const std::string &message)
{
  if (token.type == TokenType::TOK_EOF)
    report (token.line, " at end", message);
  else
    report (token.line, " at '" + token.lexeme + "'", message);
}

void
report (int line, const std::string &where, const std::string &message)
{
  std::cout << "[line " << line << "] Error" + where + ": " + message << "\n";
  had_error = true;
}

void
run_time_error (RunTimeError error)
{
  std::cout << error.what () << "\n[line " << error.token.line << "]\n";
  had_run_time_error = true;
}

RunTimeError::RunTimeError (Token token, const char *message)
    : std::runtime_error (message), token (std::move (token))
{
}
} // namespace lox
