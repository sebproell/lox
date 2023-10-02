#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sysexits.h>

#include "error.h"
#include "scanner.h"
#include "token.h"

namespace lox
{

std::string
read_file (const char *file_name)
{
  std::ifstream in (file_name);
  std::stringstream whole_file;
  whole_file << in.rdbuf ();
  return whole_file.str ();
}

void
run (const std::string &source)
{
  Scanner scanner{ source };
  auto tokens = scanner.scan_tokens ();
  for (const auto &t : tokens)
    std::cout << t.to_string ();
  std::cout << std::endl;
}

void
run_script (const char *file)
{
  run (read_file (file));
}

void
run_prompt ()
{
  std::string line;
  for (;;)
    {
      std::getline (std::cin, line);
      if (line.empty ())
        break;
      run (line);
      had_error = false;
    }
}

void
dump_tokens (const char *source)
{
  run (read_file (source));
}

void
run_special (char **argv, const char *toggle, void (*fun) (const char *source))
{
  if (strcmp (argv[1], toggle) == 0)
    fun (argv[2]);
}

} // namespace lox

int
main (int argc, char **argv)
{
  if (argc > 3)
    {
      std::cout << "Usage: cpplox [--tokens] [script]\n";
      std::exit (EX_USAGE);
    }
  if (argc == 3)
    {
      lox::run_special (argv, "--tokens", &lox::dump_tokens);
    }
  else if (argc == 2)
    {
      lox::run_script (argv[1]);
    }
  else
    {
      lox::run_prompt ();
    }

  return lox::had_error ? EX_DATAERR : EX_OK;
}
