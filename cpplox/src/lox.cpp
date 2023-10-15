#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sysexits.h>

#include "ast_printer.h"
#include "error.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"

namespace lox
{

enum class Mode
{
  dump_tokens,
  dump_ast,
};

std::string
read_file (const char *file_name)
{
  std::ifstream in (file_name);
  std::stringstream whole_file;
  whole_file << in.rdbuf ();
  return whole_file.str ();
}

void
run (const std::string &source, Mode mode)
{
  if (mode == Mode::dump_tokens)
    {
      Scanner scanner{ source };
      auto tokens = scanner.scan_tokens ();
      for (const auto &t : tokens)
        std::cout << t.to_string ();
      std::cout << std::endl;
    }
  else if (mode == Mode::dump_ast)
    {
      Scanner scanner{ source };
      auto tokens = scanner.scan_tokens ();
      Parser parser{ tokens };
      Expr expr = parser.parse ();
      std::cout << print_ast (expr) << std::endl;
    }
}

void
run_script (const char *file, Mode mode)
{
  run (read_file (file), mode);
}

void
run_prompt (Mode mode)
{
  std::string line;
  for (;;)
    {
      std::getline (std::cin, line);
      if (line.empty ())
        break;
      run (line, mode);
      had_error = false;
    }
}

std::pair<Mode, const char *>
find_mode_file (int argc, char **argv)
{
  Mode mode = Mode::dump_tokens;
  const char *file{ nullptr };

  for (int i = 1; i < argc; ++i)
    {
      const auto *current_arg = argv[i];
      if (strcmp (current_arg, "--ast") == 0)
        mode = Mode::dump_ast;
      else if (strcmp (current_arg, "--tokens") == 0)
        mode = Mode::dump_tokens;
      else
        file = current_arg;
    }

  return { mode, file };
}

} // namespace lox

int
main (int argc, char **argv)
{
  if (argc > 3)
    {
      std::cout << "Usage: cpplox [--tokens|--ast] [script]\n";
      std::exit (EX_USAGE);
    }

  const auto [mode, file] = lox::find_mode_file (argc, argv);

  if (file)
    lox::run_script (file, mode);
  else
    lox::run_prompt (mode);

  return lox::had_error ? EX_DATAERR : EX_OK;
}
