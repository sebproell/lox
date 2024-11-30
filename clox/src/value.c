#include "value.h"
#include "memory.h"
#include <stdio.h>

bool
values_equal (Value lhs, Value rhs)
{
  if (lhs.type != rhs.type)
    return false;
  switch (lhs.type)
    {
    case VAL_BOOL:
      return AS_BOOL (lhs) == AS_BOOL (rhs);
    case VAL_NIL:
      return true;
    case VAL_NUMBER:
      return (AS_NUMBER (lhs) == AS_NUMBER (rhs));
      break;
    }
}

void
init_value_array (ValueArray *array)
{
  array->count = 0;
  array->capacity = 0;
  array->values = NULL;
}

void
write_value_array (ValueArray *array, Value value)
{
  if (array->capacity < array->count + 1)
    {
      int old_capacity = array->capacity;
      array->capacity = GROW_CAPACITY (old_capacity);
      array->values
          = GROW_ARRAY (Value, array->values, old_capacity, array->capacity);
    }
  array->values[array->count] = value;
  array->count++;
}

void
free_value_array (ValueArray *array)
{
  FREE_ARRAY (Value, array->values, array->capacity);
  init_value_array (array);
}

void
print_value (Value value)
{
  switch (value.type)
    {
    case VAL_BOOL:
      printf (AS_BOOL (value) ? "true" : "false");
      break;
    case VAL_NIL:
      printf ("nil");
      break;
    case VAL_NUMBER:
      printf ("%g", AS_NUMBER (value));
      break;
    }
}
