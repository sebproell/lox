#pragma once

#include "chunk.h"

typedef struct
{
  Chunk *chunk;
  uint8_t *ip;
} VM;

typedef enum
{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_ERROR,
} InterpretResult;

void init_vm (VM *vm);
void free_vm (VM *vm);
InterpretResult interpret (VM *vm, Chunk *chunk);
