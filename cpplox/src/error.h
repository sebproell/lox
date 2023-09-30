#pragma once

#include <string>
namespace lox
{

extern bool had_error;

void report (int line, const std::string &where, const std::string &message);

void error (int line, std::string message);
}
