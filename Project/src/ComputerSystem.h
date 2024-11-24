

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

//Valeriia
//Struct Message
//To send message to Operator Console in case of collision
struct Message {
    int planeID;      // ID of the plane
    char alert[256];  // Alert message
};



class ComputerSystem {
public:
	ComputerSystem(int numPlanes);
	virtual ~ComputerSystem();
	void startSystemThread();
	pthread_t getSystemThread() const;
	void collisionTest();

	//Valeriia
	//notify operator Console that there is collision
	//the function should be invoked in collisionTest() when there is collision
//	void emitAlert(int planeID, const char* message);


private:
    int shm_fd;
    int numofPlanes;
    airplane* shared_data;
    bool running;

    pthread_t ComputerSystem_thread;
	static void* collision(void* arg);



};

#endif /* SRC_COMPUTERSYSTEM_H_ */
