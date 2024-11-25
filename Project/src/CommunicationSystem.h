

#ifndef SRC_COMMUNICATIONSYSTEM_H_
#define SRC_COMMUNICATIONSYSTEM_H_

#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>

#include "planeManager.h"
#include "airplane.h"
#include <string>


class CommunicationSystem {
public:
	CommunicationSystem(planeManager & );
	virtual ~CommunicationSystem();

	//start Thread and get Thread
	void startCommunicationThread();
	pthread_t getCommunicationThread() const;


private:

	 planeManager * manager;

	 bool running;                 // Indicates if the server is running

	 pthread_t communicationThread;
	 //to start as server
	 static void* startServerThread(void* arg);
};

#endif /* SRC_COMMUNICATIONSYSTEM_H_ */
