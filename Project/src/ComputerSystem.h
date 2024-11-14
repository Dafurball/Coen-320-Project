/*
 * ComputerSystem.h
 *
 *  Created on: Nov. 14, 2024
 *      Author: dafur
 */

#ifndef SRC_COMPUTERSYSTEM_H_
#define SRC_COMPUTERSYSTEM_H_

#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>

#include "radar.h"
#include "airplane.h"

class ComputerSystem {
public:
	ComputerSystem(int numPlanes);
	virtual ~ComputerSystem();
	void startSystemThread();
	void collisionTest();
	void printPlanes();


private:
    int shm_fd;
    int numofPlanes;
    airplane* shared_data;

    bool running;
    pthread_t ComputerSystem_thread;
	static void* collision(void* arg);



};

#endif /* SRC_COMPUTERSYSTEM_H_ */
