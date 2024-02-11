#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

VM vm;

int
main (int argc, const char **argv)
{
  init_vm (&vm);

  Chunk chunk;
  init_chunk (&chunk);

  int constant = add_constant (&chunk, 1.23);
  int constant2 = add_constant (&chunk, 2.56);
  int dummy_line = 123;
  write_chunk (&chunk, OP_CONSTANT, dummy_line);
  write_chunk (&chunk, constant, dummy_line);
  write_chunk (&chunk, OP_NEGATE, dummy_line);

  write_chunk (&chunk, OP_CONSTANT, dummy_line);
  write_chunk (&chunk, constant2, dummy_line);

  write_chunk (&chunk, OP_MULTIPLY, dummy_line);

  write_chunk (&chunk, OP_RETURN, dummy_line);

  interpret (&vm, &chunk);

  free_vm (&vm);
  free_chunk (&chunk);
  return 0;
}
