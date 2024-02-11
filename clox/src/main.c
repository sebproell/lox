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
  write_chunk (&chunk, OP_CONSTANT, 123);
  write_chunk (&chunk, constant, 123);

  write_chunk (&chunk, OP_RETURN, 123);

  interpret (&vm, &chunk);

  free_vm (&vm);
  free_chunk (&chunk);
  return 0;
}
