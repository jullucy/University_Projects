#include <stdlib.h>
#include "common/io.h"
#include "common/sumset.h"


// Constants
enum {
    ERROR = 1,
    POOL_BLOCK_SIZE = 1000 // Number of Ref_sumset structures per block
};


/*
 * Structures for stack memory management.
 */

// Structure for Ref_sumset
typedef struct Ref_sumset {
    Sumset this_sumset;       // Pointer to the sumset
    int ref_count;             // Reference count for memory management
    struct Ref_sumset* parent; // Pointer to the parent sumset
    struct Ref_sumset* next;   // Pointer to the next free node (for pooling)
} Ref_sumset;

// Memory pool for Ref_sumset
typedef struct {
    Ref_sumset* free_list;      // Head of the free list
    Ref_sumset* pool_blocks;    // Linked list of allocated blocks
} RefSumsetPool;

// Structure to manage the stack.
typedef struct {
    Ref_sumset* a;
    Ref_sumset* b;
} StackFrame;

/*
 * Functions for memory pool management.
 */

// Initialize the memory pool
void pool_init(RefSumsetPool* pool) {
    pool->free_list = NULL;
    pool->pool_blocks = NULL;
}

// Allocate a new block of Ref_sumset structures
void pool_allocate_block(RefSumsetPool* pool) {
    Ref_sumset* block = malloc(sizeof(Ref_sumset) * POOL_BLOCK_SIZE);
    if (!block) {
        exit(ERROR);
    }

    // Add the block to the pool blocks list
    block->next = (Ref_sumset*)pool->pool_blocks;
    pool->pool_blocks = block;

    // Add all nodes in the block to the free list
    for (int i = 0; i < POOL_BLOCK_SIZE; i++) {
        block[i].next = pool->free_list;
        pool->free_list = &block[i];
    }
}

// Allocate a Ref_sumset from the pool
Ref_sumset* pool_allocate(RefSumsetPool* pool) {
    if (!pool->free_list) {
        pool_allocate_block(pool); // Allocate a new block if free list is empty
    }

    Ref_sumset* node = pool->free_list;
    pool->free_list = node->next;
    node->next = NULL; // Clear the next pointer
    return node;
}

// Release a Ref_sumset back to the pool
void pool_release(RefSumsetPool* pool, Ref_sumset* node) {
    node->next = pool->free_list;
    pool->free_list = node;
}

// Free all memory used by the pool
void pool_destroy(RefSumsetPool* pool) {
    Ref_sumset* block = pool->pool_blocks;

    while (block) {
        Ref_sumset* next_block = (Ref_sumset*)block->next;
        free(block);
        block = next_block;
    }

    pool->free_list = NULL;
    pool->pool_blocks = NULL;
}

/*
 * Functions for sumsets management.
 */

// Increment the reference count
static inline void sumset_retain(Ref_sumset* sumset) {
    if (sumset) {
        sumset->ref_count++;
    }
}

// Decrement the reference count and release if it reaches 0
static inline void sumset_release(RefSumsetPool* pool, Ref_sumset* sumset) {
    while (sumset && --sumset->ref_count == 0) {
        Ref_sumset* parent = sumset->parent;
        pool_release(pool, sumset);
        sumset = parent;
    }
}


/*
 * Solve the problem iteratively.
 */
void solve_iterative(Sumset* start_a, Sumset* start_b, Solution* best_solution, InputData* input_data, RefSumsetPool* pool) {
    size_t stack_capacity = 1000;
    StackFrame* stack = malloc(sizeof(StackFrame) * stack_capacity);
    size_t stack_size = 0;

    Ref_sumset* a_beg = pool_allocate(pool);
    Ref_sumset* b_beg = pool_allocate(pool);

    a_beg->this_sumset = *start_a;
    b_beg->this_sumset = *start_b;
    a_beg->ref_count = 2;
    b_beg->ref_count = 2;
    a_beg->parent = NULL;
    b_beg->parent = NULL;

    stack[stack_size++] = (StackFrame){a_beg, b_beg};

    while (stack_size > 0) {
        StackFrame frame = stack[--stack_size];
        Ref_sumset *a, *b;

        if (frame.a->this_sumset.sum > frame.b->this_sumset.sum) {
            a = frame.b;
            b = frame.a;
        } else {
            a = frame.a;
            b = frame.b;
        }

        if (is_sumset_intersection_trivial(&a->this_sumset, &b->this_sumset)) {
            for (int i = a->this_sumset.last; i <= input_data->d; ++i) {
                if (!does_sumset_contain(&b->this_sumset, i)) {
                    Ref_sumset* new_node = pool_allocate(pool);
                    sumset_add(&new_node->this_sumset, &a->this_sumset, i);

                    new_node->parent = a;
                    new_node->ref_count = 1;

                    sumset_retain(a);
                    sumset_retain(b);

                    if (stack_size >= stack_capacity) {
                        stack_capacity *= 2;
                        stack = realloc(stack, sizeof(StackFrame) * stack_capacity);

                        if (!stack) {
                            exit(ERROR);
                        }
                    }

                    stack[stack_size++] = (StackFrame){new_node, b};
                }
            }
        } else if ((a->this_sumset.sum == b->this_sumset.sum) && (get_sumset_intersection_size(&a->this_sumset, &b->this_sumset) == 2)) {
            if (b->this_sumset.sum > best_solution->sum) {
                solution_build(best_solution, input_data, &a->this_sumset, &b->this_sumset);
            }
        }

        sumset_release(pool, a);
        sumset_release(pool, b);
    }

    free(stack);
}

int main() {
    InputData input_data;
    input_data_read(&input_data);

    Solution best_solution;
    solution_init(&best_solution);

    RefSumsetPool pool;
    pool_init(&pool);

    solve_iterative(&input_data.a_start, &input_data.b_start, &best_solution, &input_data, &pool);

    solution_print(&best_solution);
    pool_destroy(&pool);
    return 0;
}
