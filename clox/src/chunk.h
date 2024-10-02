#pragma once

#include "common.h"
#include "value.h"

typedef enum
{
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN,
} OpCode;

typedef struct
{
  int count;
  int capacity;
  uint8_t *code;
  int *lines;
  ValueArray constants;
} Chunk;

void init_chunk (Chunk *chunk);
void write_chunk (Chunk *chunk, uint8_t byte, int line);
void free_chunk (Chunk *chunk);

/// Add constant and return an index for later retrieval.
int add_constant (Chunk *chunk, Value value);
