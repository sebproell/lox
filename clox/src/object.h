#pragma once

#include "common.h"
#include "value.h"

typedef enum
{
  OBJ_STRING,
} ObjType;

struct Obj
{
  ObjType type;
};

#define OBJ_TYPE(value) (AS_OBJ (value)->type)

#define IS_STRING(value) (is_obj_type (value, OBJ_STRING))

#define AS_STRING(value) ((ObjString *)AS_OBJ (value))
#define AS_CSTRING(value) (AS_STRING (value)->chars)

typedef struct
{
  Obj obj;
  // length not including the null-terminator
  int length;
  // null-terminated C string
  char *chars;
} ObjString;

static inline bool
is_obj_type (Value value, ObjType obj_type)
{
  return IS_OBJ (value) && OBJ_TYPE (value) == obj_type;
}

/**
 * Copies the string into a new null-terminated string.
 */
ObjString *copy_string (const char *chars, int length);

/**
 * Uses the existing chars w/o copying and wraps them into a string value.
 */
ObjString *view_string (char *chars, int length);

void print_object (Value value);
