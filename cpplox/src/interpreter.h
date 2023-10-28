#include "expr.h"
#include "token.h"
#include <cassert>
#include <cstddef>
#include <string>
#include <variant>

namespace lox
{


/**
 * The central interpret call: given an expression, evaluate it and print the
 * result or report an error.
 */
void interpret (const Expr &expression);

} // namespace lox
