#include "netkit.h"
#include "assert.h"

#include "heap.h"

static void test_empty_heap(void) {
    _heap_t heap;
    _heap_init(&heap);
    ASSERT(_heap_size(&heap) == 0);

    _heap_entry_t entry;
    ASSERT(_heap_peek(&heap, &entry) == -1);
    ASSERT(_heap_pop(&heap, &entry) == -1);

    _heap_deinit(&heap);
}

static void test_insert_and_pop_single(void) {
    _heap_t heap;
    _heap_init(&heap);

    int dummy = 42;
    ASSERT(_heap_insert(&heap, 100, &dummy) == 0);
    ASSERT(_heap_size(&heap) == 1);

    _heap_entry_t entry;
    ASSERT(_heap_peek(&heap, &entry) == 0);
    ASSERT(entry.key == 100);
    ASSERT(entry.data == &dummy);

    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 100);
    ASSERT(_heap_size(&heap) == 0);

    _heap_deinit(&heap);
}

static void test_min_ordering(void) {
    _heap_t heap;
    _heap_init(&heap);

    int a = 1, b = 2, c = 3;
    ASSERT(_heap_insert(&heap, 300, &c) == 0);
    ASSERT(_heap_insert(&heap, 100, &a) == 0);
    ASSERT(_heap_insert(&heap, 200, &b) == 0);

    _heap_entry_t entry;
    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 100);
    ASSERT(entry.data == &a);

    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 200);
    ASSERT(entry.data == &b);

    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 300);
    ASSERT(entry.data == &c);

    ASSERT(_heap_size(&heap) == 0);
    _heap_deinit(&heap);
}

static void test_remove_by_data(void) {
    _heap_t heap;
    _heap_init(&heap);

    int a = 1, b = 2, c = 3;
    _heap_insert(&heap, 100, &a);
    _heap_insert(&heap, 200, &b);
    _heap_insert(&heap, 300, &c);

    ASSERT(_heap_remove(&heap, &b) == 0);
    ASSERT(_heap_size(&heap) == 2);

    _heap_entry_t entry;
    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 100);

    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 300);

    _heap_deinit(&heap);
}

static void test_remove_not_found(void) {
    _heap_t heap;
    _heap_init(&heap);

    int a = 1, b = 2;
    _heap_insert(&heap, 100, &a);
    ASSERT(_heap_remove(&heap, &b) == -1);
    ASSERT(_heap_size(&heap) == 1);

    _heap_deinit(&heap);
}

int main(void) {
    test_empty_heap();
    test_insert_and_pop_single();
    test_min_ordering();
    test_remove_by_data();
    test_remove_not_found();
    return 0;
}
