

#include "ResourceProtection.h"

sem_t shared_access;                      // Semaphore for access control
pthread_mutex_t reader_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for reader_mutex
int numofReaders = 0;                         // Initialize reader count to 0

// Initialization function to set up semaphores and mutexes
void initializeResourceProtection() {
    sem_init(&shared_access, 0, 1);  // Initialize semaphore to 1
}

// Cleanup function to destroy semaphores and mutexes
void cleanupSharedResources() {
    sem_destroy(&shared_access);
    pthread_mutex_destroy(&reader_mutex);
}

