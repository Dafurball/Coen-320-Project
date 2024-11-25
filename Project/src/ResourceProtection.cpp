

#include "ResourceProtection.h"


mutex cout_mutex;						  //As name implies, protects control of output
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// Initialization function to set up semaphores
void initializeResourceProtection(int numofPlanes) {
    pthread_rwlock_init(&rwlock, nullptr);
    sem_init(&airplane_semaphore, 0, numofPlanes);
    sem_init(&radar_semaphore, 0, 0);
    sem_init(&collision_semaphore, 0, 0);
}

// Cleanup function to destroy semaphores and mutexes
void cleanupSharedResources() {
    sem_destroy(&airplane_semaphore);
    sem_destroy(&radar_semaphore);
    sem_destroy(&collision_semaphore);
    pthread_rwlock_destroy(&rwlock);

}

void redirectToTerminal(const char* terminal) {
    int fd = open(terminal, O_WRONLY);
    if (fd != -1) {
        dup2(fd, STDOUT_FILENO); // Redirect `std::cout` to the specified terminal
        close(fd);
    } else {
        perror("Failed to open terminal");
    }
}

