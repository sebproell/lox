#pragma once

#include "token.h"
#include <stdexcept>
#include <string>
#include <utility>
namespace lox
{

extern bool had_error;
extern bool had_run_time_error;

/**
 * Run time error which knows at which Token an error occurred.
 */
class RunTimeError : public std::runtime_error
{
public:
  RunTimeError (Token token, const char *message);

  Token token;
};

void report (int line, const std::string &where, const std::string &message);

void error (int line, const std::string &message);
void error (Token token, const std::string &message);
void run_time_error (RunTimeError error);

} // namespace lox
