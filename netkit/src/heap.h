/** Copyright (c) 2026, abc <xxx@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

_Pragma("once")

#include <stdint.h>
#include <stddef.h>

typedef struct _heap_entry_s {
    uint64_t key;   /**< Sort key (expiration time in ms). */
    void*    data;  /**< Opaque pointer to the timer handle. */
} _heap_entry_t;

typedef struct _heap_s {
    _heap_entry_t* entries;   /**< Dynamic array of entries. */
    size_t         size;      /**< Number of entries currently in the heap. */
    size_t         capacity;  /**< Allocated capacity of entries array. */
} _heap_t;

/**
 * @brief Initialize a heap. Caller owns the _heap_t memory.
 *
 * @param heap  Pointer to the heap to initialize.
 */
extern void _heap_init(_heap_t* heap);

/**
 * @brief Free internal allocations. Does not free the heap struct itself.
 *
 * @param heap  Pointer to the heap to clean up.
 */
extern void _heap_deinit(_heap_t* heap);

/**
 * @brief Insert an entry into the heap.
 *
 * @param heap  The heap.
 * @param key   Sort key (lower = higher priority).
 * @param data  Opaque data pointer.
 *
 * @return 0 on success, -1 on allocation failure.
 */
extern int _heap_insert(_heap_t* heap, uint64_t key, void* data);

/**
 * @brief Peek at the minimum entry without removing it.
 *
 * @param heap  The heap.
 * @param out   Output entry. Only valid if function returns 0.
 *
 * @return 0 if heap is non-empty, -1 if empty.
 */
extern int _heap_peek(_heap_t* heap, _heap_entry_t* out);

/**
 * @brief Remove and return the minimum entry.
 *
 * @param heap  The heap.
 * @param out   Output entry. Only valid if function returns 0.
 *
 * @return 0 if an entry was removed, -1 if heap was empty.
 */
extern int _heap_pop(_heap_t* heap, _heap_entry_t* out);

/**
 * @brief Remove the first entry whose data pointer matches.
 *
 * @param heap  The heap.
 * @param data  The data pointer to search for.
 *
 * @return 0 if found and removed, -1 if not found.
 */
extern int _heap_remove(_heap_t* heap, void* data);

/**
 * @brief Return the number of entries in the heap.
 *
 * @param heap  The heap.
 *
 * @return Number of entries.
 */
extern size_t _heap_size(_heap_t* heap);
