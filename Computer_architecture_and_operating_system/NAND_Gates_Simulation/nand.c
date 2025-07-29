/**
 * Implementation of nand library.
 *
 * Credit program for "Computer architecture and operating systems" classes.
 *
 * Uses errno library.
 *
 * Compilation using given makefile.
 *
 * It is aa dynamically loaded library supporting combinational Boolean
 * circuits composed of NAND gates.
 *
 * Programme enables usage of functions declared in nand.h file:
 * nand_new - creates new nand gate.
 * nand_delete - deletes given nand gate.
 * nand_connect_nand - connects signal of nand gate exit to nand gate entry.
 * nand_connect_signal - connects bool signal to nand gate entry.
 * nand_evaluate - counts length of critical path.
 * nand_fan_out - counts the number of gate entries connected to the output of
 * a given gate.
 * nand_input - returns what is connected to entry of given gate.
 * nand_output - enables to iterate over the gates connected to the output of
 * the given gate.
 *
 * author: Julia £ucyn
 * date: 20.04.2024
 */

#include <stdlib.h>
#include <errno.h>
#include "nand.h"

#define ERROR (-1)
#define SUCCESS 0
#define CONTINUE 1

enum in_type {
    T_BOOL, // Type for bool value.
    T_NAND, // Type for nand_t value.
    T_NONE, // Type for empty entries.
};

typedef enum in_type in_type_t;

struct in {
    void* element; // The pointer for structure, not defined if type is T_NONE.
    in_type_t type; // The type of this structure.
};

typedef struct in in_t;

struct nand {
    in_t* entries; // Signals that are connected to entries of the gate
    nand_t** gates_connected_to_exit;
    unsigned* number_of_connected_entry;
    unsigned exit_size; // Size of array 'gates_connected_to_exit'.
    unsigned entry_size; // Size of array 'entries'.
    bool exit_signal; // Signal that is in the gate exit.
    bool was_visited; // Was the gate visited in current call of nand_evaluate.
    ssize_t critical_path; // Only valid if was_visited is true.
};

static void set_values_for_new_gate(nand_t *new_gate, unsigned n);
static void remove_output_signals(nand_t *remove_from);
static int manage_memory_error(nand_t *remove_from, unsigned *temp1,
                               nand_t **temp2);
static void free_entry(nand_t *remove_from, unsigned *temp1,
                       nand_t **temp2, unsigned size, nand_t *g,
                       unsigned removed_entry);
static int disconnect_entry_from_exit_array(nand_t *g, unsigned removed_entry);
static void remove_input_signals(nand_t *remove_from);
static int connect_entry_to_exit_array(nand_t *g_out, nand_t *g_in,
                                       unsigned k);
static int check_conditions(nand_t *g, ssize_t *current_path);
static int count_critical_path(nand_t *g, ssize_t *current_path);
static int check_if_new_max_path(nand_t *g, bool *s, ssize_t *max_path);
static void change_was_visited_to_false (nand_t *g);


/* definitions of local helper functions */

/**
 * Sets default values for given gate, according to its size.
 * @param new_gate pointer to the structure representing NAND gate.
 * @param n is a number of entries that new gate should have.
 */
static void set_values_for_new_gate(nand_t *const new_gate, unsigned const n) {
    for (unsigned i = 0; i < n; i++) {
        new_gate->entries[i].type = T_NONE;
    }

    new_gate->entry_size = n;
    new_gate->exit_size = 0;

    new_gate->exit_signal = false;
    new_gate->was_visited = false;

    new_gate->gates_connected_to_exit = NULL;
    new_gate->number_of_connected_entry = NULL;
}

/**
 * @param remove_from gate from which output signals will be removed.
 */
static void remove_output_signals(nand_t *const remove_from) {
    for (unsigned i = 0; i < (remove_from->exit_size); i++) {
        nand_t *gate_to_disconnect = remove_from->gates_connected_to_exit[i];
        unsigned which_entry = remove_from->number_of_connected_entry[i];

        if (gate_to_disconnect != NULL) {
            gate_to_disconnect->entries[which_entry].type = T_NONE;
        }
    }
}

/**
 * Removes all exit signals of the gate.
 * @param remove_from gate which exits will be removed.
 * @return Always returns -1, because it is only called if there was
 * a memory error.
 */
static int manage_memory_error(nand_t *const remove_from, unsigned *temp1,
                               nand_t **temp2) {
    if (temp1 != NULL) {
        free(temp1);
    }

    if (temp2 != NULL) {
        free(temp2);
    }

    remove_output_signals(remove_from);

    free(remove_from->gates_connected_to_exit);
    free(remove_from->number_of_connected_entry);

    remove_from->exit_size = 0;
    remove_from->gates_connected_to_exit = NULL;
    remove_from->number_of_connected_entry = NULL;

    return ERROR;
}

/**
 * Frees memory allocated for given entry in gates_connected_to_exit array.
 * @param remove_from gate from which space in array will be deleted.
 * @param size size of array gates_connected_to_exit array minus one.
 * @param g pointer to NAND gate which entry we want to disconnect.
 * @param removed_entry number of entry which we want to disconnect.
 */
static void free_entry(nand_t *const remove_from, unsigned *const temp1,
                       nand_t **const temp2, unsigned const size,
                       nand_t *const g, unsigned const removed_entry) {
    unsigned index = 0;

    // We are copying given arrays, without entry which we are removing.
    for (unsigned i = 0; i <= size; i++) {
        if (remove_from->gates_connected_to_exit[i] != g ||
            remove_from->number_of_connected_entry[i] != removed_entry) {
            temp1[index] = remove_from->number_of_connected_entry[i];
            temp2[index] = remove_from->gates_connected_to_exit[i];
            index++;
        }
    }

    free(remove_from->gates_connected_to_exit);
    free(remove_from->number_of_connected_entry);
}

/**
 * @param g pointer to the structure representing NAND gate.
 * @param removed_entry index of entry in entries array, which is currently
 * being disconnected from signal.
 * @return 0 if operation was successful or -1 if there was any error connected
 * with memory allocation.
 */
static int disconnect_entry_from_exit_array(nand_t *const g,
                                            unsigned const removed_entry) {
    nand_t *remove_from = (nand_t*) g->entries[removed_entry].element;

    if ((remove_from->exit_size) == 0) {
        return SUCCESS;
    }

    unsigned size = remove_from->exit_size - 1;
    unsigned *temp1 = malloc(size * sizeof(unsigned));
    nand_t **temp2 = malloc(size  * sizeof(nand_t*));

    if (temp1 == NULL || temp2 == NULL) {
        return manage_memory_error(remove_from, temp1, temp2);
    }

    free_entry(remove_from, temp1, temp2, size, g, removed_entry);

    remove_from->gates_connected_to_exit = temp2;
    remove_from->number_of_connected_entry = temp1;
    remove_from->exit_size = size;

    return SUCCESS;
}

/**
 * @param remove_from gate from which input signals will be removed.
 */
static void remove_input_signals(nand_t *const remove_from) {
    for (unsigned index = 0; index < (remove_from->entry_size); index++) {
        if (remove_from->entries[index].type == T_NAND) {
            int was_successful = disconnect_entry_from_exit_array(remove_from,
                                                                  index);

            if (was_successful == ERROR) {
                errno = ENOMEM;
            }
        }
    }
}

/**
 * Connects k entries of g_in gate to g_out gate.
 * @param g_out pointer to the structure representing NAND gate.
 * @param g_in pointer to the structure representing NAND gate.
 * @param k number of gate entries.
 * @return 0 if operation was successful or -1 if there was any error connected
 * with memory allocation.
 */
static int connect_entry_to_exit_array(nand_t *const g_out, nand_t *const g_in,
                                       unsigned const k) {
    g_out->exit_size++;
    unsigned size = g_out->exit_size;

    g_out->gates_connected_to_exit = realloc(g_out->gates_connected_to_exit,
                                             size * sizeof(nand_t*));
    g_out->number_of_connected_entry = realloc(g_out->number_of_connected_entry
                                               , size * sizeof(unsigned));

    if (g_out->gates_connected_to_exit == NULL ||
        g_out->number_of_connected_entry == NULL) {
        g_out->exit_size = 0;
        return ERROR;
    }

    g_out->gates_connected_to_exit[size - 1] = g_in;
    g_out->number_of_connected_entry[size - 1] = k;

    return SUCCESS;
}

/**
 * Checks special cases possible while counting critical path.
 * @param g pointer to the structure representing NAND gate.
 * @param current_path length of critical path counted for current gate.
 * @return 1 if conditions for counting new critical path are fulfilled,
 * 0 if critical path was counted or this gate, or its the end of counting,
 * and is correct. -1 if critical path is not correct.
 */
static int check_conditions(nand_t *const g, ssize_t *const current_path) {
    if (g->was_visited == true) {
        if (g->critical_path != ERROR) {
            *current_path += (g->critical_path);

            return SUCCESS;
        }

        return ERROR;
    }

    if (g->entry_size == 0) {
        g->critical_path = 0;
        g->was_visited = true;
        g->exit_signal = false;

        return SUCCESS;
    }

    return CONTINUE;
}

/**
 * Iterates through gates connected to entry of given gates and counts the
 * critical path.
 * @param g pointer to the structure representing NAND gate.
 * @param this_path counter for current length of critical path.
 * @param max_path the longest critical path from given gate to, when bool
 * signal or gate without entries occurs.
 * @return
 */
static int count_path(nand_t *const g, ssize_t *const this_path,
                      ssize_t *const max_path) {
    for (unsigned i = 0; i < g->entry_size; i++) {
        if (g->entries[i].type == T_NONE) {
            g->critical_path = ERROR;

            return ERROR;
        }

        if (g->entries[i].type == T_BOOL) {
            bool *this_signal = (bool*) g->entries[i].element;

            if (!(*this_signal)) {
                g->exit_signal = true;
            }
        }
        // If g->entries[i].type == T_NAND.
        else {
            nand_t *this_entry = (nand_t*) g->entries[i].element;
            *this_path = 0;

            int is_correct = count_critical_path(this_entry, this_path);

            if (is_correct == ERROR) {
                g->critical_path = ERROR;

                return ERROR;
            }

            if (*this_path > *max_path) {
                *max_path = *this_path;
            }

            if (this_entry->exit_signal == false) {
                g->exit_signal = true;
            }
        }
    }

    return SUCCESS;
}

/**
 * Menages counting the length of critical path for gate 'g'.
 * @param g pointer to the structure representing NAND gate.
 * @param current_path length of critical path counted for current gate.
 * @return 0 if operation was successful, -1 if there was any error.
 */
static int count_critical_path(nand_t *const g, ssize_t *const current_path) {
    int is_correct = check_conditions(g, current_path);

    if (is_correct != CONTINUE) {
        return is_correct;
    }

    ssize_t max_path = 0;
    ssize_t this_path = 0;
    g->critical_path = -1;
    g->exit_signal = false;
    g->was_visited = true;

    is_correct = count_path(g, &this_path, &max_path);

    if (is_correct == ERROR) {
        return ERROR;
    }

    g->critical_path = max_path++;
    *current_path += max_path++;

    return SUCCESS;
}

/**
 * Checks if the critical path counted for gate g is longer than
 * current max length of critical path.
 * @param g pointer to the structure representing NAND gate.
 * @param s pointer to bool value.
 * @param max_path length of the longest critical path found.
 * @return -1 ig there was an error, 0 if not.
 */
static int check_if_new_max_path(nand_t *const g, bool *const s,
                                 ssize_t *const max_path) {
    ssize_t current_path = 0;
    int was_successful = count_critical_path(g, &current_path);

    if (was_successful == ERROR) {
        errno = ECANCELED;
        return ERROR;
    }

    if (current_path > *max_path) {
        *max_path = current_path;
    }

    *s = g->exit_signal;

    return SUCCESS;
}

/**
 * Changes 'was_visited' to false in all gates previously visited.
 * @param g pointer to array of pointers to structures representing NAND gates.
 */
static void change_was_visited_to_false (nand_t *const g) {
    g->was_visited = false;

    for (unsigned i = 0; i < g->entry_size; i++) {
        if (g->entries[i].type == T_NAND) {
            nand_t *this_entry = (nand_t*) g->entries[i].element;
            change_was_visited_to_false(this_entry);
        }
    }
}


/* definitions of nand library functions */

/**
 * Creates new NAND gate.
 * @param n is a number of entries that new gate should have.
 * @return returns pointer to the structure representing NAND gate, or NULL if
 * there was an error connected with memory allocation.
 */
nand_t* nand_new(unsigned n) {
    nand_t *const new_gate = malloc(sizeof *new_gate);

    if (new_gate == NULL) {
        errno = ENOMEM;
    }
    else {
        new_gate->entries = malloc(n * sizeof(struct in));

        if (new_gate->entries == NULL) {
            free(new_gate);
            errno = ENOMEM;
            return NULL;
        }

        set_values_for_new_gate(new_gate, n);
    }

    return new_gate;
}

/**
 * Disconnects the input and output signals of the specified gate, then removes
 * the specified gate and frees all memory used by it. Does nothing if called
 * with a NULL pointer.
 * @param g pointer to the structure representing NAND gate.
 */
void nand_delete(nand_t *g) {
    if (g != NULL) {
        remove_input_signals(g);

        remove_output_signals(g);

        if (g->gates_connected_to_exit != NULL) {
            free(g->gates_connected_to_exit);
        }

        if (g->number_of_connected_entry != NULL) {
            free(g->number_of_connected_entry);
        }

        free(g->entries);
        free(g);
    }
}

/**
 * Connects exit of 'g_out' gate to entries of 'g_in' gate. If any signal is
 * currently connected to this entries, disconnects it.
 * @param g_out pointer to the structure representing NAND gate.
 * @param g_in pointer to the structure representing NAND gate.
 * @param k number of gate entries.
 * @return 0 if operation was successful or -1 if there was any error.
 */
int nand_connect_nand(nand_t *g_out, nand_t *g_in, unsigned k) {
    if (g_out == NULL || g_in == NULL || k + 1 > (g_in->entry_size)) {
        errno = EINVAL;
        return ERROR;
    }

    if (g_in->entries[k].type == T_NAND) {
        int was_successful = disconnect_entry_from_exit_array(g_in, k);

        if (was_successful == ERROR) {
            errno = ENOMEM;
            return ERROR;
        }
    }

    int was_successful = connect_entry_to_exit_array(g_out, g_in, k);

    if (was_successful == ERROR) {
        errno = ENOMEM;
        return ERROR;
    }

    g_in->entries[k].element = g_out;
    g_in->entries[k].type = T_NAND;

    return SUCCESS;
}

/**
 * Connects bool signal to entries of 'g_in' gate. If any signal is currently
 * connected to this entries, disconnects it.
 * @param s pointer to bool variable.
 * @param g pointer to the structure representing NAND gate.
 * @param k number of gate entries.
 * @return 0 if operation was successful or -1 if there was any error.
 */
int nand_connect_signal(bool const *s, nand_t *g, unsigned k) {
    if (s == NULL || g == NULL || k > ((g->entry_size) - 1)) {
        errno = EINVAL;
        return ERROR;
    }

    if (g->entries[k].type == T_NAND) {
        disconnect_entry_from_exit_array(g, k);
    }

    g->entries[k].type = T_BOOL;
    g->entries[k].element = (void*) s;

    return SUCCESS;
}

/**
 * @param g pointer to array of pointers, to structures representing
 * NAND gates.
 * @param s pointer to array in which designated signals should be stored.
 * @param m size of arrays pointed to by 's' and 'g'.
 * @return length of critical path if operation was successful or -1 if there
 * was any error.
 */
ssize_t nand_evaluate(nand_t **g, bool *s, size_t m) {
    if (g == NULL || s == NULL || m == 0) {
        errno = EINVAL;
        return ERROR;
    }

    ssize_t max_path = 0;

    // While iterating through given array we have to check if any pointer
    // in it is NULL.
    for (size_t i = 0; i < m; i++) {
        if (g[i] == NULL) {
            errno = EINVAL;
            return ERROR;
        }

        int was_successful = check_if_new_max_path(g[i], &s[i], &max_path);

        if(was_successful == ERROR) {
            errno = ECANCELED;
            return ERROR;
        }
    }

    for (size_t i = 0; i < m; i++) {
        change_was_visited_to_false(g[i]);
    }

    return max_path;
}

/**
 * Checks how many gates are connected to the exit of input gate.
 * @param g pointer to the structure representing NAND gate.
 * @return number of gates connected to the exit of 'g' gate or -1 if 'g' is
 * a NULL pointer.
 */
ssize_t nand_fan_out(nand_t const *g) {
    if (g == NULL) {
        errno = EINVAL;
        return ERROR;
    }

    return (ssize_t) g->exit_size;
}

/**
 * @param g pointer to the structure representing NAND gate.
 * @param k number of gate entries.
 * @return pointer to signal connected to gate entries or NULL if nothing is
 * connected to gate entries or if there was any error.
 */
void* nand_input(nand_t const *g, unsigned k) {
    if (g == NULL || k > ((g->entry_size) - 1)) {
        errno = EINVAL;
        return NULL;
    }

    if (g->entries[k].type == T_NONE) {
        errno = 0;
        return NULL;
    }

    if (g->entries[k].type == T_BOOL) {
        return (bool*) g->entries[k].element;
    }
    else {
        return (nand_t*) g->entries[k].element;
    }
}

/**
 * @param g pointer to the structure representing NAND gate.
 * @param k index between zero and value, which is lower by one then the
 * function 'nand_fan_out' result.
 * @return pointer to the gate connected to the output of gate 'g'
 * which is under index 'k' in 'gates_connected_to_exit' array.
 */
nand_t* nand_output(nand_t const *g, ssize_t k) {
    return g->gates_connected_to_exit[k];
}
