#pragma once

#include "common.h"

#define ALLOCATE(type, count)                                                 \
  (type *)reallocate (NULL, 0, sizeof (type) * (count))

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, data, old_count, new_count)                          \
  (type *)reallocate ((data), sizeof (type) * (old_count),                    \
                      sizeof (type) * (new_count))

#define FREE(type, pointer) reallocate (pointer, sizeof (type), 0)

#define FREE_ARRAY(type, pointer, old_count)                                  \
  reallocate (pointer, sizeof (type) * (old_count), 0)

/**
 * Resize @p data of @p old_size to fit the @p new_size. @p new_size may be
 * zero, indicating a deletion of the buffer.
 */
void *reallocate (void *data, size_t old_size, size_t new_size);
