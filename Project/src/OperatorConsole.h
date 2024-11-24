

#ifndef SRC_OPERATORCONSOLE_H_
#define SRC_OPERATORCONSOLE_H_


#include <string>
#include <vector>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#include "airplane.h"




class OperatorConsole {

public:
	OperatorConsole(int numPlanes);
	virtual ~OperatorConsole();


	// Function that sends a command to aircraft (to request airplane to change its course)
//	void sendCommand(int planeID, int newSpeedX, int newSpeedY, int newSpeedZ, int newX, int newY, int newZ);


private:
	int shm_fd;
	int numofPlanes;

	airplane* shared_data;//THIS is pointer to shared memory for all the airplanes

	bool running;

	//
	sem_t* console_semaphore;  // Semaphore for synchronizing operations


};

#endif /* SRC_OPERATORCONSOLE_H_ */
