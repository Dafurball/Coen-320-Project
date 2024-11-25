

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

#include "airplane.h"

#include <string>

class CommunicationSystem {
public:
	CommunicationSystem();
	virtual ~CommunicationSystem();

	//start Thread and get Thread
	void startCommunicationThread();
	pthread_t getCommunicationThread() const;



private:

	 planeManager * manager;

	 static void* serverThread(void* arg);

	 bool running;                 // Indicates if the server is running
	 pthread_t serverThreadHandle;
};

#endif /* SRC_COMMUNICATIONSYSTEM_H_ */
