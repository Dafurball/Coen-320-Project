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
#include "planeManager.h"
#include "Command.h"



class OperatorConsole {

public:
	OperatorConsole(planeManager & );
	virtual ~OperatorConsole();


	//Start the thread
	void startOperatorConsoleThread();

	// Function that sends a command to aircraft (to request airplane to change its course)
	void sendCommand(int aircraftID, const std::string& command, int valueX, int valueY);


	//get the thread
	pthread_t getconsoleThread() const;


private:
	planeManager * manager;
	pthread_t consoleThread;


	// Flag to control the running state of the thread
	std::atomic<bool> runningOperatorConsole;

	//Function processes commands received & sends them in message to ComputerSystem
	static void* processCommands(void* arg);

	//Function reads commands from controller
	void handleCommands();



};

#endif /* SRC_OPERATORCONSOLE_H_ */
