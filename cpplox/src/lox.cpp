#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sysexits.h>

bool had_error = false;

void
run (const std::string &source)
{
  // TODO
  std::cout << source;
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
    run_script (argv[1]);
  else
    run_prompt ();

  return had_error ? EX_DATAERR : EX_OK;
}
