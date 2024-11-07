#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "airplane.h"
#include "printer.h"


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

		//Creating shared memory from airplane data
		int shm_fd = shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);  // Open or create shared memory
	    if (shm_fd == -1) {
	        perror("shm_open failed");
	        return 1;
	    }

	    //Memory mapping, this will be by the size of each airplane object multiplied by the index (number of airplane objects created
	    airplane* shm_data = (airplane*)mmap(0, sizeof(airplane) * index, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	    if (shm_data == MAP_FAILED) {
	        perror("memory mapping failed");
	        return 1;
	    }

	    // Copying data from airplane to the shared memory.  Note that even though the shared memory is different from the data of the airplane objects, it still
	    //follows protection files (public, private).  Will need set/get methods for access later on!
	    for (int i = 0; i < index; i++) {
	        memcpy(&shm_data[i], airplanes[i], sizeof(airplane));
	    }

	    //Quick print test to see if shared memory bit worked correctly
	    for (int i = 0; i < index; ++i) {
	        std::cout<< " Position: (" << shm_data[i].get_x() << ", " << shm_data[i].get_y() << ", " << shm_data[i].get_z() << ")"
	                  << " Speed: (" << shm_data[i].get_speedX() << ", " << shm_data[i].get_speedY() << ", " << shm_data[i].get_speedZ() << ")"
	                  << std::endl;
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

    munmap(shm_data, sizeof(airplane) * index);  // Unmap shared memory
    close(shm_fd);  // Close the file descriptor


	return 0;
}
