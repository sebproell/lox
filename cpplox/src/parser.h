#pragma once

#include "stmt.h"
#include "token.h"

#include <memory>
#include <vector>

namespace lox
{
namespace internal
{
class ParserImpl;
}

class Parser
{
public:
  Parser (std::vector<Token> tokens);

  /**
   * Destructor.
   */
  ~Parser ();

  /**
   * Parse the sequence of Tokens in this Parser into a program, i.e., a series
   * of statements.
   */
  std::vector<Stmt> parse ();

private:
  std::unique_ptr<internal::ParserImpl> pimpl;
};

} // namespace lox
