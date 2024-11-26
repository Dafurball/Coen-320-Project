
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

#include <queue>
#include <mutex>

class ComputerSystem {
public:
	ComputerSystem(int numPlanes);
	virtual ~ComputerSystem();

	//Start threads
	void startSystemThread();
	void startComms();
	void startCommunicationThread();

	//get threads
	pthread_t getSystemThread() const;
	pthread_t getComThread() const;
	pthread_t getToCommunication_thread() const;

	//Perform collision test
	void collisionTest();


private:
    int shm_fd;
    int numofPlanes;
    airplane* shared_data;

    bool running_collision;
    bool running_coms;
    int delta = 1;	//for changeTimeBetweenCollision

    //3 threads for Computer System
    pthread_t ComputerSystem_thread;	//collision detection
    pthread_t comms_thread;				//OperatorSystem to ComputerSystem channel
    pthread_t toCommunication_thread;	//ComputerSystem to CommunicationSystem channel


	static void* collision(void* arg);

	//Start server to listen for Operator's commands
	static void* startServer(void *arg);

	//process operator's commands
	static void* processCommandsToCommunication(void *arg);
	void processCommandsToCommunication(int aircraftID, const std::string& command, int value);


};

#endif /* SRC_COMPUTERSYSTEM_H_ */
