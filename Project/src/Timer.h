
#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

#include <csignal>
#include <ctime>
#include <iostream>
#include <mutex>

// Global mutex for synchronized console output
extern std::mutex cout_mutex;

/**
 * @brief Signal handler for timers and task management.
 * @param sig Signal number received.
 * @param si Pointer to signal information (contains user-defined data).
 * @param context Unused parameter (required for SA_SIGINFO).
 */
void signalHandler(int sig, siginfo_t* si, void* context);

/**
 * @brief Create a POSIX timer.
 * @param timerId Pointer to the timer ID to be created.
 * @param signal Signal number to trigger when the timer expires.
 * @param intervalSeconds Timer interval in seconds.
 * @param data Pointer to user-defined data passed to the signal handler.
 */
void createTimer(timer_t* timerId, int signal, int intervalSeconds, void* data);

/**
 * @brief Initialize signal handlers for the program.
 * Registers handlers for SIGUSR1, SIGUSR2, and SIGRTMIN+ signals.
 */
void initializeSignalHandler();
#endif /* SRC_TIMER_H_ */
