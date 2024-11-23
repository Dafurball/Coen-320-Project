

#include "ResourceProtection.h"

//sem_t shared_access;                      // Semaphore for access control
mutex cout_mutex;						  //As name implies, protects control of output
//pthread_mutex_t reader_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for reader_mutex
//int numofReaders = 0;                         // Initialize reader count to 0
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// Initialization function to set up semaphores
void initializeResourceProtection(int numofPlanes) {
 //   sem_init(&shared_access, 0, 1);  // Initialize semaphore to 1
    pthread_rwlock_init(&rwlock, nullptr);
    sem_init(&airplane_semaphore, 0, numofPlanes);
        sem_init(&radar_semaphore, 0, 0);
        sem_init(&collision_semaphore, 0, 0);

        global_time = 0;


}

// Cleanup function to destroy semaphores and mutexes
void cleanupSharedResources() {
   // sem_destroy(&shared_access);
 //   pthread_mutex_destroy(&reader_mutex);
}

