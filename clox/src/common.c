#include "common.h"

CommandLineOptions global_options = 0;

bool
is_option_set (CommandLineOptions option)
{
  return global_options & option;
}

void
set_option (CommandLineOptions options)
{
  global_options |= options;
}
