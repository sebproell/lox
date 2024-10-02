#pragma once

#include "token.h"
#include <string_view>
#include <vector>

namespace lox
{
/**
 * Split the @p source into tokens.
 */
std::vector<Token> scan_tokens (std::string_view source);

} // namespace lox
