

#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "radar.h"
#include "airplane.h"
#include "ResourceProtection.h"

using namespace std;





//radar::radar() {
	// TODO Auto-generated constructor stub

//}

radar::~radar() {
    stopRadarThread();
    stopAirplaneThreads();
    delete[] airplanes;
    delete[] airplane_threads;

    if (shared_data) {
        munmap(shared_data, sizeof(airplane) * numofPlanes);
    }
    if (shared_fd != -1) {
        close(shared_fd);
        shm_unlink("/airplane_data");
    }
}

// Constructor to load planes from file, as well as to set up shared memory and load planes to it
radar::radar(const string& filename) // @suppress("Class members should be properly initialized")
    : airplanes(nullptr), numofPlanes(0), shared_fd(-1), shared_data(nullptr),
      airplane_threads(nullptr), running(false) {

	//File location is in the VM target, not windows!  PATH: /data/var/tmp
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    //Will keep track of how many airplane objects are created
    string line;
    while (getline(inputFile, line)) {
        if (!line.empty()) numofPlanes++;
    }

    // Allocate array of airplanes based on number of planes (numofPlanes)
    airplanes = new airplane[numofPlanes];
    airplane_threads = new pthread_t[numofPlanes];

    // Resets cursor of file from when we found out number of planes
    inputFile.clear();
    inputFile.seekg(0);

	//Standard airplane object creation from contents of a file
    int index = 0;
    while (getline(inputFile, line) && index < numofPlanes) {
        istringstream iss(line);
        int time, id, x, y, z, speedX, speedY, speedZ;
        if (iss >> time >> id >> x >> y >> z >> speedX >> speedY >> speedZ) {
            airplanes[index] = airplane(time, id, x, y, z, speedX, speedY, speedZ);
            index++;
        }
    }

	//Creating shared memory for airplane data
    shared_fd = shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (shared_fd == -1) {
        perror("shm_open failed");
        return;
    }

    //Truncate the shared memory to the correct size (big errors if not included I found out lol)
    if (ftruncate(shared_fd, sizeof(airplane) * numofPlanes) == -1) {
        perror("ftruncate failed");
        return;
    }

    //Memory mapping, this will be by the size of each airplane object multiplied by the number of airplane objects created
    shared_data = (airplane*)mmap(0, sizeof(airplane) * numofPlanes, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
    }

    //Creates the threads of type airplane
    for (int i = 0; i < numofPlanes; i++) {
        pthread_create(&airplane_threads[i], nullptr, airplane::location_update, &shared_data[i]);
    }

    //Copying data from airplane to the shared memory.  Note that even though the shared memory is different from the data of the airplane objects, it still
    //follows protection files (public, private)!
    for (int i = 0; i < numofPlanes; ++i) {
        memcpy(&shared_data[i], &airplanes[i], sizeof(airplane));

    }
}



//Stop airplane threads
void radar::stopAirplaneThreads() {
    for (int i = 0; i < numofPlanes; i++) {
        pthread_cancel(airplane_threads[i]);
    }
}

//Start the radar thread (to be done in main.cpp unless I can find a better solution)...
void radar::startRadarThread() {
    running = true;
    pthread_create(&radar_thread, nullptr, radar::updater, this);
    pthread_join(radar_thread, nullptr);
}

//...and then end the radar thread!
void radar::stopRadarThread() {
    if (running) {
        running = false;
    }
}

// Print all loaded airplanes (for testing)
void * radar::updater(void * arg) {
	radar* radarObj = static_cast<radar*>(arg);
	    while (radarObj->running) {

	  //      m.lock();

            radarObj->printPlanes(); // Call the existing printPlanes method

       //     m.unlock();

	        sleep(5); // Sleep for 5 seconds before printing again
	    }
	    return nullptr;
}

/*
airplane* radar::getAirplanes() {
    return airplanes;
}
*/
int radar::getnumofPlanes() {
    return numofPlanes;
}


//Print Function
void radar::printPlanes() {
	for (int i = 0; i < numofPlanes; ++i) {

//////////////////////////////////////////Reader Lock///////////////////////////////////////////////
	    pthread_mutex_lock(&reader_mutex);
	    numofReaders++;
	    if (numofReaders == 1) {  // First reader turns on the light...
	        sem_wait(&shared_access);
	    }
	    pthread_mutex_unlock(&reader_mutex);

////////////////////////////////////////Critical Section///////////////////////////////////////////
		cout<< "ID: " << shared_data[i].get_id() << " Time: " << shared_data[i].get_time() << " Position: (" << shared_data[i].get_x() << ", " << shared_data[i].get_y() << ", " << shared_data[i].get_z() << ")"
				  << " Speed: (" << shared_data[i].get_speedX() << ", " << shared_data[i].get_speedY() << ", " << shared_data[i].get_speedZ() << ")"
				  << endl << flush;

//////////////////////////////////////////Reader Unlock//////////////////////////////////////////////
		pthread_mutex_lock(&reader_mutex);
		numofReaders--;
		if (numofReaders == 0) {  // ...Last reader shuts off the light!
			sem_post(&shared_access);
		}
		pthread_mutex_unlock(&reader_mutex);
	}
}


