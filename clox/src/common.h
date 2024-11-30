#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Options that can be passed to the command line and modify clox behavior.
 */
typedef enum
{
  OPT_TRACE_EXECUTION = 1,
  OPT_TOKENS = 2,
  OPT_DISASSEMBLE = 4,
  OPT_NO_EXECUTION = 8,
} CommandLineOptions;

bool is_option_set (CommandLineOptions option);

void set_option (CommandLineOptions options);
