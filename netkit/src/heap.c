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

#include "heap.h"

#include <stdlib.h>

#define _HEAP_INITIAL_CAP 16

/* Swap two heap entries. */
static void _heap_swap(_heap_entry_t* a, _heap_entry_t* b) {
    _heap_entry_t tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Bubble entry at idx up to restore heap property. */
static void _heap_sift_up(_heap_t* heap, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (heap->entries[idx].key < heap->entries[parent].key) {
            _heap_swap(&heap->entries[idx], &heap->entries[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

/* Push entry at idx down to restore heap property. */
static void _heap_sift_down(_heap_t* heap, size_t idx) {
    for (;;) {
        size_t smallest = idx;
        size_t left     = 2 * idx + 1;
        size_t right    = 2 * idx + 2;

        if (left < heap->size &&
            heap->entries[left].key < heap->entries[smallest].key) {
            smallest = left;
        }
        if (right < heap->size &&
            heap->entries[right].key < heap->entries[smallest].key) {
            smallest = right;
        }
        if (smallest == idx) {
            break;
        }
        _heap_swap(&heap->entries[idx], &heap->entries[smallest]);
        idx = smallest;
    }
}

void _heap_init(_heap_t* heap) {
    heap->entries  = NULL;
    heap->size     = 0;
    heap->capacity = 0;
}

void _heap_deinit(_heap_t* heap) {
    free(heap->entries);
    heap->entries  = NULL;
    heap->size     = 0;
    heap->capacity = 0;
}

int _heap_insert(_heap_t* heap, uint64_t key, void* data) {
    if (heap->size == heap->capacity) {
        size_t new_cap = (heap->capacity == 0) ? _HEAP_INITIAL_CAP
                                               : heap->capacity * 2;
        _heap_entry_t* new_entries = (_heap_entry_t*)realloc(
            heap->entries,
            new_cap * sizeof(_heap_entry_t));
        if (!new_entries) {
            return -1;
        }
        heap->entries  = new_entries;
        heap->capacity = new_cap;
    }
    heap->entries[heap->size].key  = key;
    heap->entries[heap->size].data = data;
    heap->size++;
    _heap_sift_up(heap, heap->size - 1);
    return 0;
}

int _heap_peek(_heap_t* heap, _heap_entry_t* out) {
    if (heap->size == 0) {
        return -1;
    }
    *out = heap->entries[0];
    return 0;
}

int _heap_pop(_heap_t* heap, _heap_entry_t* out) {
    if (heap->size == 0) {
        return -1;
    }
    *out = heap->entries[0];
    heap->size--;
    if (heap->size > 0) {
        heap->entries[0] = heap->entries[heap->size];
        _heap_sift_down(heap, 0);
    }
    return 0;
}

int _heap_remove(_heap_t* heap, void* data) {
    size_t idx;
    for (idx = 0; idx < heap->size; idx++) {
        if (heap->entries[idx].data == data) {
            break;
        }
    }
    if (idx == heap->size) {
        return -1;
    }
    heap->size--;
    if (idx < heap->size) {
        heap->entries[idx] = heap->entries[heap->size];
        _heap_sift_down(heap, idx);
        _heap_sift_up(heap, idx);
    }
    return 0;
}

size_t _heap_size(_heap_t* heap) {
    return heap->size;
}
