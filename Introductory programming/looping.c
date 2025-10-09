/**
 * "Looping"
 *
 * Credit program for "Introduction to Programming" classes.
 *
 * Compilation command:
 *
 * gcc @options looping.c -o looping
 *
 * Program enables implementation of Looper language.
 *
 * Reads and executes commands:
 * printing variable values;
 * executing programs in Looping language.
 */


#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>


/* array manipulations */

/**
 * Creates array of variables.
 */
int **create_array(void) {
  int **arr;
  arr = (int**) malloc(26 * sizeof(int*));
  for (int i = 0; i < 26; ++i) {
    arr[i] = (int*) calloc(3, sizeof(int));
  }
  return arr;
}

/**
 * Creates array of sizes.
 */
int *create_sizes_array(void) {
  int *sizes_arr;
  sizes_arr = (int*) malloc(26 * sizeof(int));
  for (int i = 0; i < 26; ++i) {
    sizes_arr[i] = 3;
  }
  return sizes_arr;
}

/**
 * Frees memory allocated for array 'arr'.
 */
void free_array(int **arr) {
  for (int i = 0; i < 26; ++i) {
    free(arr[i]);
  }
  free(arr);
}

/**
 * Doubles the size of the array if possible.
 *
 * If not possible, changes the array size to INT_MAX.
 */
void expand(char **arr, int *size) {
  int temp = *size;
  if (2 * (*size) < INT_MAX) {
    *size = 2 * (*size);
  }
  else {
    *size = INT_MAX;
  }
  *arr = (char*) realloc (*arr, (unsigned int) *size * sizeof(char));
  while (temp < *size) {
    (*arr)[temp] = 0;
    ++temp;
  }
}

/**
 * Increases the size of array 'arr' by one.
 */
void expand_by_one(int **arr, int *size) {
  *size = *size + 1;
  *arr = (int*) realloc (*arr, (unsigned int) *size * sizeof(int));
  (*arr)[*size - 1] = 0;
}

/**
 * Changes the size of array 'arr' from 'size' to 'new_size'.
 */
void resize_array(int **arr, int *size, int new_size) {
  int temp = *size;
  *arr = (int*) realloc (*arr, (unsigned int) new_size * sizeof(int));
  while (temp < new_size) {
    (*arr)[temp] = 0;
    ++temp;
  }
  *size = new_size;
}


/* variable operations */

/**
 * Adds one to the variable stored in array 'variable'.
 *
 * 'size' is the size of array 'variable'.
 */
void add_one(int **variable, int *size) {
  int i = 0;
  int carry = 1;
  while (carry != 0) {
    if (i == *size) {
      expand_by_one(variable, size);
    }
    (*variable)[i] = (*variable)[i] + 1;
    carry = 0;
    if ((*variable)[i] == 10) {
      (*variable)[i] = 0;
      carry = 1;
    }
    ++i;
  }
}

/**
 * Passes the appropriate variable to function 'add_one'.
 */
int add_operation(int ***variables, int **sizes, char **command, int i) {
  char variable = (*command)[i];
  ++i;
  int which = variable - 'a';
  add_one(&(*variables)[which], &(*sizes)[which]);
  return i;
}

/**
 * Decreases the value of variable 'variable' by one.
 */
void decrease(int **variable) {
  int i = 0;
  while ((*variable)[i] == 0) {
    (*variable)[i] = 9;
    ++i;
  }
  (*variable)[i] = (*variable)[i] - 1;
}

/**
 * Resets variable 'variable' to zero.
 */
void reset_to_zero(int **variable, int *size) {
  *variable = realloc (*variable, 3 * sizeof(int));
  (*variable)[0] = 0;
  (*variable)[1] = 0;
  (*variable)[2] = 0;
  *size = 3;
}

/**
 * Adds value from array 'source' to array 'target'.
 *
 * 'source_size' is the size of array 'source'.
 *
 * 'target_size' is the size of array 'target'.
 */
void transfer(int **source, int **target, int *source_size, int *target_size) {
  int carry = 0;
  int i = 0;
  if (*source_size > *target_size) {
    resize_array(target, target_size, *source_size);
  }
  while (i < *source_size) {
    (*target)[i] = (*target)[i] + (*source)[i] + carry;
    if ((*target)[i] >= 10) {
      (*target)[i] = (*target)[i] - 10;
      carry = 1;
    }
    else {
      carry = 0;
    }
    ++i;
  }
  while (carry > 0) {
    if (i == *target_size) {
      expand_by_one(target, target_size);
    }
    (*target)[i] = (*target)[i] + 1;
    if ((*target)[i] >= 10) {
      (*target)[i] = (*target)[i] - 10;
      carry = 1;
    }
    else {
      carry = 0;
    }
    ++i;
  }
}


/* code operations */

/**
 * Checks if variable value is equal to 0.
 */
bool is_end(int **variables, int count) {
  bool signal = false;
  int i = 0;
  while (i < count && !signal) {
    if ((*variables)[i] > 0) {
      signal = true;
    }
    ++i;
  }
  return signal;
}

/**
 * Finds the end of parenthetical expression and returns index directly after it.
 */
int find_parenthesis_end(char **command, int i) {
  int paren_count = 1;
  while (paren_count > 0) {
    if ((*command)[i] == ')') {
      --paren_count;
    }
    else if ((*command)[i] == '(') {
      ++paren_count;
    }
    ++i;
  }
  return --i;
}

/**
 * Checks what kind of code to generate.
 *
 * Returns true if optimized code should be generated.
 *
 * Returns false if regular code should be generated.
 */
bool which_code(char **command, int index) {
  bool signal = true;
  int i = index;
  while ((*command)[i] != ')' && signal)  {
    if ((*command)[i] == '(') {
      signal = false;
    }
    ++i;
    if ((*command)[i] == (*command)[index]) {
      signal = false;
    }
  }
  return signal;
}


/* looper program execution */

/**
 * Executes optimized code.
 */
void optimized_code(int ***variables, int **sizes, char **command, int i) {
    char temp = (*command)[i];
    int index = temp - 'a';
    ++i;
    while ((*command)[i] != ')') {
      transfer(&(*variables)[index], &(*variables)[(*command)[i] - 'a'],
      &(*sizes)[index], &(*sizes)[(*command)[i] - 'a']);
      ++i;
    }
    reset_to_zero(&(*variables)[index], &(*sizes)[index]);

}

/**
 * Executes repeat instruction.
 */
int repeat(int ***variables, int **sizes, char **command, int i) {
  int index = i;
  if (which_code(command, index)) {
    optimized_code(variables, sizes, command, i);
  }
  else {
    int end;
    while (is_end(&((*variables)[(*command)[i] - 'a']),
    (*sizes)[(*command)[i] - 'a'])) {
      decrease(&((*variables)[(*command)[i] - 'a']));
      end = find_parenthesis_end(command, i + 1);
      index = i + 1;
      while (index < end) {
        if ((*command)[index] == '(') {
          ++index;
          index = repeat(variables, sizes, command, index);
        }
        else {
          index = add_operation(variables, sizes, command, index);
        }
      }
    }
  }
  i = find_parenthesis_end(command, i + 1) + 1;
  return i;
}

/**
 * Prints variable after reading '='.
 */
void print_variable(int arr[], int size) {
   int i = size - 1;
   while (i >= 0 && arr[i] == 0) --i;
   while (i > 0) {
     printf("%d", arr[i]);
     --i;
   }
   printf("%d", arr[0]);
   printf("\n");
}

/**
 * Reads command.
 */
void read_command(char **arr, int *arr_size) {
  int i = 0;
  int c = getchar();
  while (c != '\n') {
    if (i == *arr_size) {
      expand(arr, arr_size);
    }
    (*arr)[i] = (char) c;
    ++i;
    c = getchar();
  }
  if (i == *arr_size) {
    expand(arr, arr_size);
  }
  (*arr)[i] = (char) c;
}

/**
 * Executes looper language commands.
 */
void execute_commands(int ***variables, int **sizes, char **command) {
  int i = 0;
  while ((*command)[i] != '\n') {
    if ((*command)[i] == '(') {
      ++i;
      i = repeat(variables, sizes, command, i);
    }
    else {
      i = add_operation(variables, sizes, command, i);
    }
  }
}

/**
 * Executes looper program.
 */
void run_program(void) {
  int arr_size = 3;
  char *command = (char*) malloc((unsigned int) arr_size * sizeof(char));
  int **variables = create_array();
  int *sizes = create_sizes_array();
  int c = getchar();
  while (c != EOF) {
    if (c == '=') {
      c = getchar();
      int index = c - 'a';
      int size = sizes[index];
      print_variable(variables[index], size);
      c = getchar();
    }
    else {
      ungetc(c, stdin);
      read_command(&command, &arr_size);
      execute_commands(&variables, &sizes, &command);
    }
    c = getchar();
    arr_size = 3;
    command = (char*) realloc(command,(unsigned int) arr_size * sizeof(char));
  }
  free(command);
  free_array(variables);
  free(sizes);
}

int main(void) {
  run_program();
  return 0;
}
