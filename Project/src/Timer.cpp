#include "Timer.h"
#include "radar.h"
#include "ComputerSystem.h"
#include "airplane.h"
#include <iostream>
#include <mutex>
#include <cstdlib> // For exit()

// Define the global mutex for synchronized console output

/**
 * @brief Signal handler implementation.
 */
void signalHandler(int sig, siginfo_t* si, void* context) {
    if (sig == SIGUSR1) {
        // Radar update
        radar* radarSystem = static_cast<radar*>(si->si_value.sival_ptr);
        radarSystem->printPlanes();
    } else if (sig == SIGUSR2) {
        // Collision detection
        ComputerSystem* compSystem = static_cast<ComputerSystem*>(si->si_value.sival_ptr);
        compSystem->collisionTest();
    } else if (sig >= SIGRTMIN) {
        // Airplane location update
        airplane* plane = static_cast<airplane*>(si->si_value.sival_ptr);
        plane->new_location();
    }
}

/**
 * @brief Create a POSIX timer.
 */
void createTimer(timer_t* timerId, int signal, int intervalSeconds, void* data) {
    struct sigevent sev;
    struct itimerspec its;

    // Configure the signal event
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = signal;
    sev.sigev_value.sival_ptr = data; // User-defined data passed to the signal handler

    if (timer_create(CLOCK_REALTIME, &sev, timerId) == -1) {
        perror("Failed to create timer");
        exit(EXIT_FAILURE);
    }

    // Configure the timer's expiration and interval
    its.it_value.tv_sec = intervalSeconds; // First expiration
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = intervalSeconds; // Interval for recurring timer
    its.it_interval.tv_nsec = 0;

    if (timer_settime(*timerId, 0, &its, nullptr) == -1) {
        perror("Failed to set timer");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Initialize signal handlers.
 */
void initializeSignalHandler() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;        // Use extended signal information
    sa.sa_sigaction = signalHandler; // Set the signal handler function
    sigemptyset(&sa.sa_mask);        // Block no additional signals while handling

    // Register handlers for radar and computer system signals
    if (sigaction(SIGUSR1, &sa, nullptr) == -1) {
        perror("Failed to set signal handler for SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &sa, nullptr) == -1) {
        perror("Failed to set signal handler for SIGUSR2");
        exit(EXIT_FAILURE);
    }

    // Register handlers for real-time signals for airplanes
    for (int i = 0; i < 64; ++i) { // Support up to 64 airplanes
        if (sigaction(SIGRTMIN + i, &sa, nullptr) == -1) {
            perror("Failed to set signal handler for SIGRTMIN");
            exit(EXIT_FAILURE);
        }
    }
}
