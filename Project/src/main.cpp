
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "airplane.h"
#include "radar.h"
#include "ComputerSystem.h"
#include "ResourceProtection.h"

int main(){

	//Sets up all the global semaphores and mutexes used in protecting critical sections
	initializeResourceProtection();

	//Set up for the Radar
	radar Radar("example.txt");
	Radar.startRadarThread();	//thread that prints data every 5 seconds

	//Set up for the Computer System
	int numPlanes = Radar.getnumofPlanes();   // Retrieve numofPlanes from Radar
	ComputerSystem system(numPlanes);
	system.startSystemThread();

	//Now know that having threads join in the main is the wya to go, or else you block the other threads...  TIL
	pthread_join(Radar.getRadarThread(),nullptr);
	pthread_join(system.getSystemThread(),nullptr);






	return 0;
}
