#include "vm.h"
#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include <stdio.h>

static void
reset_stack (VM *vm)
{
  vm->stack_top = vm->stack;
}

void
init_vm (VM *vm)
{
  reset_stack (vm);
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
#define BINARY_OP(op)                                                         \
  do                                                                          \
    {                                                                         \
      Value b = pop (vm);                                                     \
      Value a = pop (vm);                                                     \
      push (vm, (a op b));                                                    \
    }                                                                         \
  while (false)

  for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
      printf ("          ");
      for (Value *slot = vm->stack; slot < vm->stack_top; slot++)
        {
          printf ("[ ");
          print_value (*slot);
          printf (" ]");
        }
      printf ("\n");
      disassemble_instruction (vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif
      uint8_t instruction;
      switch (instruction = READ_BYTE ())
        {
        case OP_CONSTANT:
          {
            Value constant = READ_CONSTANT ();
            push (vm, constant);
            break;
          }
        case OP_ADD:
          BINARY_OP (+);
          break;
        case OP_SUBTRACT:
          BINARY_OP (-);
          break;
        case OP_MULTIPLY:
          BINARY_OP (*);
          break;
        case OP_DIVIDE:
          BINARY_OP (/);
          break;
        case OP_NEGATE:
          {
            push (vm, -pop (vm));
            break;
          }
        case OP_RETURN:
          {
            print_value (pop (vm));
            printf ("\n");
            return INTERPRET_OK;
          }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult
interpret (VM *vm, const char *source)
{
  compile (source);
  return INTERPRET_OK;
}

void
push (VM *vm, Value value)
{
  *(vm->stack_top++) = value;
}

Value
pop (VM *vm)
{
  return *(--(vm->stack_top));
}
