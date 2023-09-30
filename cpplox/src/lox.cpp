#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sysexits.h>

#include "error.h"
#include "scanner.h"
#include "token.h"

namespace lox
{
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
  std::ifstream in (file);
  std::stringstream whole_file;
  whole_file << in.rdbuf ();
  run (whole_file.str ());
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

}

int
main (int argc, char **argv)
{
  if (argc > 2)
    {
      std::cout << "Usage: cpplox [script]\n";
      std::exit (EX_USAGE);
    }
  else if (argc == 2)
    lox::run_script (argv[1]);
  else
    lox::run_prompt ();

  return lox::had_error ? EX_DATAERR : EX_OK;
}
