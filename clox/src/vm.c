#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include <stdio.h>

void
init_vm (VM *vm)
{
}

void
free_vm (VM *vm)
{
}

static InterpretResult
run (VM *vm)
{
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE ()])

  for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
      disassemble_instruction (vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif
      uint8_t instruction;
      switch (instruction = READ_BYTE ())
        {
        case OP_CONSTANT:
          {
            Value constant = READ_CONSTANT ();
            print_value (constant);
            printf ("\n");
            break;
          }
        case OP_RETURN:
          return INTERPRET_OK;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult
interpret (VM *vm, Chunk *chunk)
{
  vm->chunk = chunk;
  vm->ip = vm->chunk->code;
  return run (vm);
}
