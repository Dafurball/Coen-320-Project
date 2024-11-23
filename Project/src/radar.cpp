

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
#include "Timer.h"

using namespace std;





//radar::radar() {
	// TODO Auto-generated constructor stub

//}

radar::~radar() {
    stopRadarThread();


    if (shared_data) {
        munmap(shared_data, sizeof(airplane) * numofPlanes);
    }
    if (shared_fd != -1) {
        close(shared_fd);
        shm_unlink("/airplane_data");
    }
}

// Constructor to load planes from file, as well as to set up shared memory and load planes to it
radar::radar(int numPlanes): numofPlanes(numPlanes), running(false) {


	//Creating shared memory for airplane data
    shared_fd = shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (shared_fd == -1) {
        perror("shm_open failed");
        return;
    }

    //Truncate the shared memory to the correct size (big errors if not done I found out lol)
    if (ftruncate(shared_fd, sizeof(airplane) * numofPlanes) == -1) {
        perror("ftruncate failed");
        return;
    }

    //Memory mapping, this will be by the size of each airplane object multiplied by the number of airplane objects created
    shared_data = (airplane*)mmap(0, sizeof(airplane) * numofPlanes, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
    }


}

//Start the radar thread (to be done in main.cpp unless I can find a better solution)...
void radar::startRadarThread() {
    running = true;
    pthread_create(&radar_thread, nullptr, radar::updater, this);
  //  pthread_join(radar_thread, nullptr);
}

pthread_t radar::getRadarThread() const {
    return radar_thread;
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
	    	sem_wait(&radar_semaphore);


            radarObj->printPlanes(); // Call the existing printPlanes method

	    }
	    return nullptr;
}



//Print Function, used for testing
void radar::printPlanes() {
	for (int i = 0; i < numofPlanes; ++i) {

//////////////////////////////////////////Reader Lock///////////////////////////////////////////////
	  /*  pthread_mutex_lock(&reader_mutex);
	    numofReaders++;
	    if (numofReaders == 1) {  // First reader turns on the light...
	        sem_wait(&shared_access);
	    }
	    pthread_mutex_unlock(&reader_mutex);*/
	pthread_rwlock_rdlock(&rwlock);

		//if (shared_data[i].get_time() < global_time){

	//	}

		//else{
////////////////////////////////////////Critical Section///////////////////////////////////////////
        std::lock_guard<std::mutex> lock(cout_mutex);
		cout<< "ID: " << shared_data[i].get_id() << " Time: " << shared_data[i].get_time() << " Position: (" << shared_data[i].get_x() << ", " << shared_data[i].get_y() << ", " << shared_data[i].get_z() << ")"
				  << " Speed: (" << shared_data[i].get_speedX() << ", " << shared_data[i].get_speedY() << ", " << shared_data[i].get_speedZ() << ")"
				  << endl << flush;

//////////////////////////////////////////Reader Unlock//////////////////////////////////////////////
		pthread_rwlock_unlock(&rwlock);
	//	}
		/*pthread_mutex_lock(&reader_mutex);
		numofReaders--;
		if (numofReaders == 0) {  // ...Last reader shuts off the light!
			sem_post(&shared_access);
		}
		pthread_mutex_unlock(&reader_mutex);*/
	}
}


