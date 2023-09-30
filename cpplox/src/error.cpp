#include "error.h"
#include <iostream>

namespace lox
{

bool had_error = true;

void
error (int line, std::string message)
{
  report (line, "", message);
}

void
report (int line, const std::string &where, const std::string &message)
{
  std::cout << "[line " << line << "] Error" + where + ": " + message << "\n";
  had_error = true;
}
}
