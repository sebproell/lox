#pragma once

#include "common.h"

typedef enum
{
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER,
} ValueType;

typedef struct
{
  ValueType type;
  union
  {
    bool boolean;
    double number;
  } as;
} Value;

#define BOOL_VAL(value) ((Value){ .type = VAL_BOOL, { .boolean = value } })
#define NIL_VAL ((Value){ .type = VAL_NIL, { .numebr = 0 } })
#define NUMBER_VAL(value) ((Value){ .type = VAL_NUMBER, { .number = value } })

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

typedef struct
{
  int capacity;
  int count;
  Value *values;
} ValueArray;

void init_value_array (ValueArray *array);
void write_value_array (ValueArray *array, Value value);
void free_value_array (ValueArray *array);

void print_value (Value value);
