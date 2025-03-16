#include <stddef.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common/io.h"
#include "common/sumset.h"
#include "common/err.h"


// Constants
enum {
    ERROR = 1,
    POOL_BLOCK_SIZE = 1000 // Number of Ref_sumset structures per block
};


/*
 * Structures for stack memory management.
 */

// Structure representing a reference-counted sumset.
typedef struct Ref_sumset {
    Sumset this_sumset;        // Pointer to the sumset
    atomic_int ref_count;      // Reference count for memory management
    struct Ref_sumset* parent; // Pointer to the parent sumset
    struct Ref_sumset* next;   // Pointer to the next free node (for pooling)
} Ref_sumset;

// Structure representing a stack frame.
typedef struct {
    Ref_sumset* a;
    Ref_sumset* b;
} StackFrame;

// Memory pool for Ref_sumset
typedef struct {
    Ref_sumset* free_list;      // Head of the free list
    Ref_sumset* pool_blocks;    // Linked list of allocated blocks
} RefSumsetPool;

// Global work queue structure.
typedef struct {
    Sumset* start_a;
    Sumset* start_b;
    StackFrame* frames;        // Dynamic array of stack frames
    size_t size;               // Current number of stack frames
    size_t capacity;           // Maximum capacity of the stack
    pthread_mutex_t mutex;     // Mutex for thread-safe operations
    pthread_cond_t cond;       // Condition variable for waiting threads
    int working_counter;       // Counter for active working threads
} WorkQueue;


// Arguments passed to each thread.
typedef struct {
    InputData* input_data;     // Input data shared among threads
    Solution* best_solution;   // Shared solution structure
    WorkQueue* work_queue;     // Pointer to the global work queue
    pthread_mutex_t* mutex;     // Mutex for thread-safe operations
} ThreadArgs;


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
 * Functions for global stack management.
 */

// Initialize the work queue.
void work_queue_init(WorkQueue* queue, size_t capacity, int t) {
    queue->frames = malloc(sizeof(StackFrame) * capacity);
    queue->size = 0;
    queue->capacity = capacity;
    queue->working_counter = t;

    ASSERT_ZERO(pthread_mutex_init(&queue->mutex, NULL));
    ASSERT_ZERO(pthread_cond_init(&queue->cond, NULL));
}

// Push a frame onto the work queue.
void work_queue_push(WorkQueue* queue, StackFrame frame) {
    ASSERT_ZERO(pthread_mutex_lock(&queue->mutex));

    // Resize the queue if necessary.
    if (queue->size >= queue->capacity) {
        queue->capacity *= 2;
        queue->frames = realloc(queue->frames, sizeof(StackFrame) * queue->capacity);

        if (!queue->frames) {
            exit(ERROR);
        }
    }
    queue->frames[queue->size++] = frame;

    ASSERT_ZERO(pthread_cond_signal(&queue->cond));
    ASSERT_ZERO(pthread_mutex_unlock(&queue->mutex));
}

// Pop a frame from the work queue.
int work_queue_pop(WorkQueue* queue, StackFrame* frame) {
    ASSERT_ZERO(pthread_mutex_lock(&queue->mutex));

    // If the queue is empty, decrease the working counter and wait for a task to be added.
    if (queue->size == 0) {
        queue->working_counter--;

        while (queue->size == 0 && queue->working_counter > 0) {
            // Wait for a task to be added or for all threads to stop working.
            ASSERT_ZERO(pthread_cond_wait(&queue->cond, &queue->mutex));
        }

        queue->working_counter++;
    }

    if (queue->size == 0) { // No more tasks and no working threads.
        ASSERT_ZERO(pthread_mutex_unlock(&queue->mutex));
        return 0;
    }

    *frame = queue->frames[--queue->size];
    ASSERT_ZERO(pthread_mutex_unlock(&queue->mutex));

    return 1;
}

// Decrease the working counter and signal waiting threads.
void working_decrease(WorkQueue* queue) {
    ASSERT_ZERO(pthread_mutex_lock(&queue->mutex));

    queue->working_counter--;

    ASSERT_ZERO(pthread_cond_signal(&queue->cond));
    ASSERT_ZERO(pthread_mutex_unlock(&queue->mutex));
}


/*
 * Functions for sumsets management.
 */

// Increment the reference count of a sumset.
void sumset_retain(Ref_sumset* sumset) {
    if (sumset) {
        atomic_fetch_add(&sumset->ref_count, 1);
    }
}

// Decrement the reference count of a sumset and free it if no references remain.
void sumset_release(RefSumsetPool* pool, Ref_sumset* sumset) {
    while (sumset) {
        atomic_int ref_count = atomic_fetch_sub(&sumset->ref_count, 1);

        if (ref_count == 1) {
            Ref_sumset* parent = sumset->parent;
            pool_release(pool, sumset);
            sumset = parent;
        } else {
            break;
        }
    }
}

/*
 * Iterative solution.
 * This function is used when the work queue isn't big enough.
 */
void solve_iteratively(Ref_sumset* a, Ref_sumset* b, InputData* input_data, Solution* best_solution, RefSumsetPool* pool, WorkQueue *work_queue) {
    if (a->this_sumset.sum > b->this_sumset.sum) {
        Ref_sumset *temp = a;
        a = b;
        b = temp;
    }

    // Check the intersection of A^\u03A3 and B^\u03A3.
    if (is_sumset_intersection_trivial(&a->this_sumset, &b->this_sumset)) {
        for (int i = a->this_sumset.last; i <= input_data->d; ++i) {
            if (!does_sumset_contain(&b->this_sumset, i)) {
                Ref_sumset* new_node = pool_allocate(pool);

                sumset_add(&new_node->this_sumset, &a->this_sumset, i);

                new_node->parent = a;
                new_node->ref_count = 1;

                sumset_retain(a);
                sumset_retain(b);

                work_queue_push(work_queue, (StackFrame){new_node, b});
            }
        }
    } else if ((a->this_sumset.sum == b->this_sumset.sum) && (get_sumset_intersection_size(&a->this_sumset, &b->this_sumset) == 2)) {
        if (b->this_sumset.sum > best_solution->sum) {
            solution_build(best_solution, input_data, &a->this_sumset, &b->this_sumset);
        }
    }
}

/*
 * Recursive solution.
 * This function is used when the work queue is big enough.
 */
void solve_recursive(const Sumset* a, const Sumset* b, InputData* input_data, Solution* best_solution) {
    if (a->sum > b->sum)
        return solve_recursive(b, a, input_data, best_solution);

    if (is_sumset_intersection_trivial(a, b)) { // s(a) ∩ s(b) = {0}.
        for (int i = a->last; i <= input_data->d; ++i) {
            if (!does_sumset_contain(b, i)) {
                Sumset a_with_i;
                sumset_add(&a_with_i, a, i);
                solve_recursive(&a_with_i, b, input_data, best_solution);
            }
        }
    } else if ((a->sum == b->sum) && (get_sumset_intersection_size(a, b) == 2)) { // s(a) ∩ s(b) = {0, ∑b}.
        if (b->sum > best_solution->sum)
            solution_build(best_solution, input_data, a, b);
    }
}

/*
 * Worker thread function.
 */
void* worker_thread(void* arg) {
    // Get the thread arguments.
    ThreadArgs* args = (ThreadArgs*)arg;
    InputData* input_data = args->input_data;
    WorkQueue* global_work_queue = args->work_queue;
    pthread_mutex_t* solution_mutex = args->mutex;

    // Initialize local variables.
    Solution best_solution;
    solution_init(&best_solution);

    StackFrame frame;
    Ref_sumset *a, *b;

    RefSumsetPool pool;
    pool_init(&pool);

    while(true) {
        if (!work_queue_pop(global_work_queue, &frame)) {
            break; // No more tasks.
        }

        a = frame.b;
        b = frame.a;

        // Solve the task iteratively or recursively, depending on the size of the work queue.
        if (global_work_queue->size < 2 * input_data->t) {
            solve_iteratively(a, b, input_data, &best_solution, &pool, global_work_queue);
        } else {
            solve_recursive(&a->this_sumset, &b->this_sumset, input_data, &best_solution);
        }

        // Release the sumsets taken from the stack.
        sumset_release(&pool, a);
        sumset_release(&pool, b);
    }

    // Update the global best solution.
    ASSERT_ZERO(pthread_mutex_lock(solution_mutex));

    if (best_solution.sum > args->best_solution->sum) {
        *args->best_solution = best_solution;
    }

    ASSERT_ZERO(pthread_mutex_unlock(solution_mutex));

    // Clean up resources.
    working_decrease(global_work_queue);
    pool_destroy(&pool);

    return 0;
}

// Main function.
int main() {
    InputData input_data;
    input_data_read(&input_data);

    Solution best_solution;
    solution_init(&best_solution);

    // Initialize the work queue.
    WorkQueue work_queue;
    work_queue_init(&work_queue, 1000, input_data.t);

    // Create initial tasks.
    Ref_sumset* a_beg = malloc(sizeof(Ref_sumset));
    Ref_sumset* b_beg = malloc(sizeof(Ref_sumset));

    a_beg->this_sumset = input_data.a_start;
    b_beg->this_sumset = input_data.b_start;
    a_beg->ref_count = 2;
    b_beg->ref_count = 2;
    a_beg->parent = NULL;
    b_beg->parent = NULL;
    work_queue_push(&work_queue, (StackFrame){a_beg, b_beg});

    // Create a mutex for solution.
    pthread_mutex_t solution_mutex;
    ASSERT_ZERO(pthread_mutex_init(&solution_mutex, NULL));

    // Create and start worker threads.
    pthread_t threads[input_data.t];
    ThreadArgs thread_args = {&input_data, &best_solution, &work_queue, &solution_mutex};

    for (size_t i = 0; i < input_data.t; ++i) {
        ASSERT_ZERO(pthread_create(&threads[i], NULL, worker_thread, &thread_args));
    }

    // Wait for all threads to finish.
    for (size_t i = 0; i < input_data.t; ++i) {
        ASSERT_ZERO(pthread_join(threads[i], NULL));
    }

    solution_print(&best_solution);

    // Free the memory.
    free(a_beg);
    free(b_beg);
    free(work_queue.frames);

    // Clean up resources.
    ASSERT_ZERO(pthread_mutex_destroy(&work_queue.mutex));
    ASSERT_ZERO(pthread_cond_destroy(&work_queue.cond));
    ASSERT_ZERO(pthread_mutex_destroy(&solution_mutex));

    return 0;
}