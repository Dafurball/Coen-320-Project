/*
 * ResourceProtection.h
 *
 *  Created on: Nov. 14, 2024
 *      Author: dafur
 */

#ifndef SRC_RESOURCEPROTECTION_H_
#define SRC_RESOURCEPROTECTION_H_

#include <pthread.h>
#include <semaphore.h>

extern sem_t shared_access;
extern pthread_mutex_t reader_mutex;
extern int numofReaders;

void initializeResourceProtection();


#endif /* SRC_RESOURCEPROTECTION_H_ */
