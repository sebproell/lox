#pragma once
#include <cstddef>
#include <string>
#include <variant>

namespace lox
{
/**
 * The types of literals that may occur in code
 */
using Literal = std::variant<std::nullptr_t, std::string, bool, double>;

/**
 * The result of interpreting a syntax tree.
 *
 * @note It just so happens that the literal types are also the types that one
 * can obtain from an expression.
 */
using Value = Literal;

/**
 * String representation of a literal.
 */
std::string to_string (const Literal &l);
} // namespace lox
