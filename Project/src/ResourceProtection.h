

#ifndef SRC_RESOURCEPROTECTION_H_
#define SRC_RESOURCEPROTECTION_H_

#include <pthread.h>
#include <semaphore.h>
#include <mutex>

using namespace std;

extern sem_t shared_access;
extern mutex cout_mutex;
extern pthread_mutex_t reader_mutex;
extern int numofReaders;

void initializeResourceProtection();


#endif /* SRC_RESOURCEPROTECTION_H_ */
