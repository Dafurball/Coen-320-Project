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
#include "printer.h"


using namespace std;

mutex m;	//mutex m that will be shared globally between airplane threads and printer thread


int main() {

	///////////////////////////////////////////Creating Airplane Objects from File Saved//////////////////////////////////////////////////////////////

	//Will need to find a way to have size dynamically created (maybe find a way to determine from size of file?)
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

	///////////////////////////////////////////////////Shared Memory Creation and Set up//////////////////////////////////////////////////////////////

	//Creating shared memory from airplane data
	int shared_fd = shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
	if (shared_fd == -1) {
		perror("shm_open failed");
		return 1;
	}

	//Memory mapping, this will be by the size of each airplane object multiplied by the index (number of airplane objects created)
	airplane* shared_data = (airplane*)mmap(0, sizeof(airplane) * index, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
	if (shared_data == MAP_FAILED) {
		perror("memory mapping failed");
		return 1;
	}

	// Copying data from airplane to the shared memory.  Note that even though the shared memory is different from the data of the airplane objects, it still
	//follows protection files (public, private).  Will need set/get methods for access later on!
	for (int i = 0; i < index; i++) {
		memcpy(&shared_data[i], airplanes[i], sizeof(airplane));
	}

	//Quick print test to see if shared memory bit worked correctly, for bug testing purposes
	for (int i = 0; i < index; ++i) {
		std::cout<< " Position: (" << shared_data[i].get_x() << ", " << shared_data[i].get_y() << ", " << shared_data[i].get_z() << ")"
				  << " Speed: (" << shared_data[i].get_speedX() << ", " << shared_data[i].get_speedY() << ", " << shared_data[i].get_speedZ() << ")"
				  << std::endl;
	}

/////////////////////////////////////////////////////////////Pthread creation and joining/////////////////////////////////////////////////////////////

	pthread_t airplane_threads[index];

	//creates the pthreads for each airplane object created
	for (int i = 0; i < index; i++){
		pthread_create(&airplane_threads[i], nullptr, airplane::location_update, &shared_data[i]);
	}

	//Have every pthread join so that they can keep running
	//for (int j = 0; j < index; j++){
		//pthread_join(airplane_threads[j], NULL);

	//}


    //Just a test to see if shared memory is being properly updated until I implement the print thread
	sleep(5);

	for (int i = 0; i < index; ++i) {
		std::cout<< " Position: (" << shared_data[i].get_x() << ", " << shared_data[i].get_y() << ", " << shared_data[i].get_z() << ")"
				  << " Speed: (" << shared_data[i].get_speedX() << ", " << shared_data[i].get_speedY() << ", " << shared_data[i].get_speedZ() << ")"
				  << std::endl;
	}


	//cleanup for when it's all done
    munmap(shared_data, sizeof(airplane) * index);
    close(shared_fd);


	return 0;
}
