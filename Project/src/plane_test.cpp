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
	        cerr << "Error opening file!" << endl;
	        return 1;
	   }

	string cursor;
	int index = 0;

	while (getline(inputFile,cursor) && (index < 100)){
		istringstream iss(cursor);
		int time, id, x, y, z, SpeedX, SpeedY, SpeedZ;

		if (iss >> time >> id >> x >> y >> z >> SpeedX >> SpeedY >> SpeedZ){
			airplanes[index] = new airplane(time, id, x, y, z, SpeedX, SpeedY, SpeedZ);
			index++;
		}

	}

	pthread_t airplane_threads[index];

	for (int i = 0; i < index; i++){
		pthread_create(&airplane_threads[i], nullptr, airplane::location_update, airplanes[i]);
	}

	for (int j = 0; j < index; j++){
		pthread_join(airplane_threads[j], NULL);

	}

	return 0;
}
