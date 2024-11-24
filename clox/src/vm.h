#pragma once

#include "chunk.h"
#include "value.h"

#define STACK_MAX 128

typedef struct
{
  Chunk *chunk;
  uint8_t *ip;
  // Stack starts out at index 0 and grows in positive direction.
  Value stack[STACK_MAX];
  // The next free entry on the stack.
  Value *stack_top;
} VM;

typedef enum
{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
  INTERPRET_ERROR,
} InterpretResult;

void init_vm (VM *vm);
void free_vm (VM *vm);
InterpretResult interpret (VM *vm, const char *source);
