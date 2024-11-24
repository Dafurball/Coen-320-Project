

#ifndef SRC_RESOURCEPROTECTION_H_
#define SRC_RESOURCEPROTECTION_H_

#include <pthread.h>
#include <semaphore.h>
#include <mutex>

using namespace std;

extern mutex cout_mutex;

extern pthread_rwlock_t rwlock;

extern sem_t airplane_semaphore;
extern sem_t radar_semaphore;
extern sem_t collision_semaphore;

void initializeResourceProtection(int);

void cleanupSharedResources();

void redirectToTerminal(const char* terminal);



#endif /* SRC_RESOURCEPROTECTION_H_ */
