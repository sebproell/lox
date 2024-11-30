#include "common.h"
#include "vm.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

VM vm;

static void
print_help ()
{
  printf ("Usage: clox [options] [path]\n");
  printf ("Options:\n");
  printf ("  --tokens\t\tPrint tokens\n");
  printf ("  --disassemble\t\tDisassemble bytecode\n");
  printf ("  --trace_execution\tTrace execution\n");
  printf ("  -n, --no_execution\tDo not execute code\n");
  printf ("  -h, --help\t\tPrint this help message\n");
}

static CommandLineOptions
parse_options (int argc, char **argv, int *parsed_argc)
{
  struct option longopts[]
      = { { "tokens", no_argument, 0, OPT_TOKENS },
          { "disassemble", no_argument, 0, OPT_DISASSEMBLE },
          { "trace_execution", no_argument, 0, OPT_TRACE_EXECUTION },
          { "no_execution", no_argument, 0, OPT_NO_EXECUTION },
          { "help", no_argument, 0, 'h' },
          { 0, 0, 0, 0 } };
  int longind, opt;
  *parsed_argc = 0;
  const char *optstring = "hn";
  CommandLineOptions options = 0;
  while ((opt = getopt_long (argc, argv, optstring, longopts, &longind)) != -1)
    {
      if (opt == 'h')
        {
          print_help ();
          exit (0);
        }
      if (opt == '?')
        {
          print_help ();
          exit (1);
        }
      if (opt == 'n')
        {
          opt = OPT_NO_EXECUTION;
        }

      options |= opt;
      *parsed_argc += 1;
    }
  return options;
}

static void
repl ()
{
  char line[1024];
  for (;;)
    {
      printf ("> ");
      if (!fgets (line, sizeof (line), stdin))
        {
          printf ("\n");
          break;
        }
      interpret (&vm, line);
    }
}

static char *
read_file (const char *path)
{
  FILE *file = fopen (path, "rb");
  if (file == NULL)
    {
      fprintf (stderr, "Could not open file \"%s\".\n", path);
      exit (74);
    }

  fseek (file, 0L, SEEK_END);
  size_t fileSize = ftell (file);
  rewind (file);

  char *buffer = (char *)malloc (fileSize + 1);
  if (buffer == NULL)
    {
      fprintf (stderr, "Not enough memory to read \"%s\".\n", path);
      exit (74);
    }

  size_t bytesRead = fread (buffer, sizeof (char), fileSize, file);
  if (bytesRead < fileSize)
    {
      fprintf (stderr, "Could not read file \"%s\".\n", path);
      exit (74);
    }
  buffer[bytesRead] = '\0';

  fclose (file);
  return buffer;
}

static void
run_file (const char *file)
{
  char *source = read_file (file);
  InterpretResult result = interpret (&vm, source);
  free (source);
  if (result == INTERPRET_COMPILE_ERROR)
    exit (65);
  if (result == INTERPRET_RUNTIME_ERROR)
    exit (70);
}

int
main (int argc, char **argv)
{
  int parsed_argc;
  CommandLineOptions options = parse_options (argc, argv, &parsed_argc);
  set_option (options);

  init_vm (&vm);

  int remaining_argc = argc - parsed_argc;
  if (remaining_argc == 1)
    repl ();
  else if (remaining_argc == 2)
    run_file (argv[parsed_argc + 1]);
  else
    {
      fprintf (stderr, "Usage: clox [options] [path]\n");
      exit (64);
    }

  free_vm (&vm);
  return 0;
}
