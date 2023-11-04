#include "stmt.h"
#include <vector>

namespace lox
{

/**
 * The central interpret call: given a program, evaluate it and print the
 * result or report an error.
 */
void interpret (const std::vector<Stmt> &program);

} // namespace lox
