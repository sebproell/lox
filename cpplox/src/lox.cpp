#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sysexits.h>

#include "ast_printer.h"
#include "error.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "stmt.h"
#include "token.h"

namespace lox
{

enum class Mode
{
  interpret,
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
  auto tokens = scan_tokens (source);

  switch (mode)
    {
    case Mode::interpret:
      {
        Parser parser{ tokens };
        std::vector<Stmt> program = parser.parse ();
        lox::interpret (program);
        break;
      }
    case Mode::dump_tokens:
      {
        for (const auto &t : tokens)
          std::cout << t.to_string ();
        std::cout << std::endl;
        break;
      }
    case Mode::dump_ast:
      {
        Parser parser{ tokens };
        std::vector<Stmt> program = parser.parse ();
        for (const auto &stmt : program)
          std::cout << print_ast (stmt) << std::endl;
        break;
      }
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
  Mode mode = Mode::interpret;
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

  if (lox::had_error)
    return EX_DATAERR;
  if (lox::had_run_time_error)
    return EX_SOFTWARE;

  return EX_OK;
}
