
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mutex.h"
#include "airplane.h"
#include "radar.h"
#include "ComputerSystem.h"

int main(){



radar Radar("example.txt");

Radar.startRadarThread();	//thread that prints data every 5 seconds
int numPlanes = Radar.getnumofPlanes();   // Retrieve numofPlanes from Radar


ComputerSystem system(numPlanes);

system.startSystemThread();






	return 0;
}
