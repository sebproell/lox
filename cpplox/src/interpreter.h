#include "stmt.h"
#include <memory>
#include <vector>

namespace lox
{
namespace internal
{
class InterpreterImpl;
}

/**
 * The interpreter evaluating and holding the state of the program.
 */
class Interpreter
{
public:
  /**
   * Constructor.
   */
  Interpreter ();

  /**
   * Destructor.
   */
  ~Interpreter ();

  /**
   * The central interpret call: given a program, evaluate it and print the
   * result or report an error.
   */
  void interpret (const std::vector<Stmt> &program);

  /**
   * Evaluate a single statement on this interpreters state. This is a useful
   * method for the REPL.
   */
  void interpret (const Stmt &stmt);

private:
  std::unique_ptr<internal::InterpreterImpl> pimpl;
};

} // namespace lox
