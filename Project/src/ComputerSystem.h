
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

#include <queue>      // For std::queue
#include <mutex>

class ComputerSystem {
public:
	ComputerSystem(int numPlanes);
	virtual ~ComputerSystem();

	//Thread 1
	void startSystemThread();
	void startComms();

	//Thread with Communication
	void startCommunicationThread();


	pthread_t getSystemThread() const;
	pthread_t getComThread() const;
	pthread_t getToCommunication_thread() const;

	void collisionTest();





private:
    int shm_fd;
    int numofPlanes;
    airplane* shared_data;

    bool running_collision;
    bool running_coms;
    // int delta = 1;

    pthread_t ComputerSystem_thread;
    pthread_t comms_thread;

    pthread_t toCommunication_thread;	//!!!


	static void* collision(void* arg);
	static void* startServer(void *arg);

	static void* processCommandsToCommunication(void *arg);	//!!!
	void processCommandsToCommunication(int aircraftID, const std::string& command, int value);


	//?????????


};

#endif /* SRC_COMPUTERSYSTEM_H_ */
