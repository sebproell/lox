#pragma once

#include "token.h"
#include <string>
namespace lox
{

extern bool had_error;

void report (int line, const std::string &where, const std::string &message);

void error (int line, const std::string &message);
void error (Token token, const std::string &message);
} // namespace lox
