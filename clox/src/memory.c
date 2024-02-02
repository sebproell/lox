#include <stdlib.h>

#include "memory.h"

void *
reallocate (void *data, size_t old_size, size_t new_size)
{
  if (new_size == 0)
    {
      free (data);
      return NULL;
    }

  void *result = realloc (data, new_size);
  if (result == NULL)
    exit (1);
  return result;
}
