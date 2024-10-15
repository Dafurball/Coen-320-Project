#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "airplane.h"

using namespace std;


int main() {

	airplane * airplanes[100];

	ifstream inputFile;

	//File location is in the VM target, not windows!  PATH: /data/var/tmp
	inputFile.open("example.txt");
	   if (!inputFile) {
	        cerr << "Error opening file" << endl;
	        return 1;
	   }

	string cursor;

	//Will keep track of how many airplane objects are created
	int index = 0;

	//Standard airplane object creation from contents of a file
	while (getline(inputFile,cursor) && (index < 100)){
		istringstream iss(cursor);

		int time, id, x, y, z, SpeedX, SpeedY, SpeedZ;

		if (iss >> time >> id >> x >> y >> z >> SpeedX >> SpeedY >> SpeedZ){
			airplanes[index] = new airplane(time, id, x, y, z, SpeedX, SpeedY, SpeedZ);
			index++;
		}

	}

	pthread_t airplane_threads[index];

	//creates the pthreads for each airplane object created
	for (int i = 0; i < index; i++){
		pthread_create(&airplane_threads[i], nullptr, airplane::location_update, airplanes[i]);
	}

	//Have every pthread join so that they can keep running
	for (int j = 0; j < index; j++){
		pthread_join(airplane_threads[j], NULL);

	}

	return 0;
}
