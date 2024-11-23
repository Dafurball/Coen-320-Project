

#ifndef SRC_RESOURCEPROTECTION_H_
#define SRC_RESOURCEPROTECTION_H_

#include <pthread.h>
#include <semaphore.h>
#include <mutex>

using namespace std;

//extern sem_t shared_access;
extern mutex cout_mutex;
//extern pthread_mutex_t reader_mutex;
//extern int numofReaders;
extern pthread_rwlock_t rwlock;

extern sem_t airplane_semaphore;
extern sem_t radar_semaphore;
extern sem_t collision_semaphore;
extern atomic<int> global_time;




void initializeResourceProtection(int);


#endif /* SRC_RESOURCEPROTECTION_H_ */
