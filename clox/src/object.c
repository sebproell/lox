#include "object.h"
#include "memory.h"
#include "string.h"
#include "value.h"
#include <stdio.h>

Obj *allocated_objs;

static void
free_object (Obj *object)
{
  switch (object->type)
    {
    case OBJ_STRING:
      {
        ObjString *string = (ObjString *)object;
        FREE_ARRAY (char, string->chars, string->length + 1);
        FREE (ObjString, string);
        break;
      }
    }
}

void
free_objects (Obj *obj_list)
{
  Obj *object = allocated_objs;
  while (object != NULL)
    {
      Obj *next = object->next;
      free_object (object);
      object = next;
    }
}

static Obj *
allocate_obj (size_t size, ObjType obj_type)
{
  // Note that this is potentially larger than Obj to accomodate the concrete
  // implementation
  Obj *object = (Obj *)reallocate (NULL, 0, size);
  object->type = obj_type;

  // Store the allocated object so we can free it.
  object->next = allocated_objs;
  allocated_objs = object;

  return object;
}

#define ALLOCATE_OBJ(type, obj_type)                                          \
  (type *)allocate_obj (sizeof (type), (obj_type))

static ObjString *
wrap_in_string_obj (char *chars, int length)
{
  ObjString *string = ALLOCATE_OBJ (ObjString, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  return string;
}

ObjString *
copy_string (const char *chars, int length)
{
  char *heap_chars = ALLOCATE (char, length + 1);
  memcpy (heap_chars, chars, length);
  heap_chars[length] = '\0';
  return wrap_in_string_obj (heap_chars, length);
}

ObjString *
view_string (char *chars, int length)
{
  return wrap_in_string_obj (chars, length);
}

void
print_object (Value value)
{
  switch (OBJ_TYPE (value))
    {
    case OBJ_STRING:
      printf ("%s", AS_CSTRING (value));
      break;
    }
}
