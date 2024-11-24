#ifndef SRC_OPERATORCONSOLE_H_
#define SRC_OPERATORCONSOLE_H_


#include <string>
#include <vector>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>
#include <unistd.h>
#include <sys/dispatch.h>
#include <thread>

#include "airplane.h"




class OperatorConsole {

public:
	OperatorConsole();
	virtual ~OperatorConsole();


	void startOperatorConsoleThread();
	// Function that sends a command to aircraft (to request airplane to change its course)


	void sendCommand(int aircraftID, const std::string& command);

	pthread_t getconsoleThread() const;


private:

	pthread_t consoleThread;
	// Flag to control the running state of the thread
	    std::atomic<bool> runningOperatorConsole;

	    // Static function to run as the pthread entry point
	    static void* processCommands(void* arg);

	    // Internal method for processing user commands
	    void handleCommands();



};

#endif /* SRC_OPERATORCONSOLE_H_ */
