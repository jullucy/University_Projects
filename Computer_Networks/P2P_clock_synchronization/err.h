/**
 * @file err.h
 * @brief Declaration of error handling functions for the synchronization protocol.
 */

#ifndef ERR_H
#define ERR_H

#include <stdnoreturn.h>

/**
 * @brief Prints information about a system error and terminates the program.
 *
 * @param fmt Format string (like in printf).
 * @param ... Additional arguments for the format string.
 */
noreturn void syserr(const char* fmt, ...);

/**
 * @brief Prints information about an error and terminates the program.
 *
 * @param fmt Format string (like in printf).
 * @param ... Additional arguments for the format string.
 */
noreturn void fatal(const char* fmt, ...);

/**
 * @brief Prints information about an error and returns to the caller.
 *
 * @param fmt Format string (like in printf).
 * @param ... Additional arguments for the format string.
 */
void error(const char* fmt, ...);

/**
 * @brief Prints information about an error message buffer and returns to the caller.
 *
 * @param msg Pointer to the message buffer.
 * @param len Length of the message buffer.
 */
void error_msg(const uint8_t *msg, size_t len);

#endif // ERR_H
