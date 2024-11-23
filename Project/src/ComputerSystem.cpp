#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <mutex>

#include "ComputerSystem.h"
#include "radar.h"
#include "airplane.h"
#include "ResourceProtection.h"

using namespace std;

ComputerSystem::ComputerSystem(int numPlanes): numofPlanes(numPlanes), running(false) {
    // Open the shared memory segment created by radar
    shm_fd= shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error opening shared memory for ComputerSystem" << std::endl;
       return;
    }

    //Truncate the shared memory to the correct size (big errors if not included I found out lol)
    if (ftruncate(shm_fd, sizeof(airplane) * numofPlanes) == -1) {
        perror("ftruncate failed");
        return;
    }


    // Map the shared memory to this process's address space (read-only)
    shared_data = static_cast<airplane*>(mmap(nullptr, sizeof(airplane) * numofPlanes, PROT_READ, MAP_SHARED, shm_fd, 0));
    if (shared_data == MAP_FAILED) {
        std::cerr << "Error mapping shared memory in ComputerSystem" << std::endl;
        close(shm_fd);
        return;
    }




}

ComputerSystem::~ComputerSystem() {
	// TODO Auto-generated destructor stub
}

void ComputerSystem::startSystemThread() {
    running = true;
    pthread_create(&ComputerSystem_thread, nullptr, collision, this);
}

pthread_t ComputerSystem::getSystemThread() const {
    return ComputerSystem_thread;
}


//the method our ComputerSystem thread calls repeatedly
void * ComputerSystem::collision(void * arg) {
	ComputerSystem* system = static_cast<ComputerSystem*>(arg);
	    while (system->running) {

	    	sem_wait(&collision_semaphore);

	    //	cout << "test" << endl;
            system->collisionTest();



	    }
	    return nullptr;
}

//Checks collisions, uses nifty trick for calculating distance without having to use sqrt function
void ComputerSystem::collisionTest() {
    const int min_horizontal = 9000000; // 3000^2
    const int min_vertical = 1000000;   // 1000^2

    for (int i = 0; i < numofPlanes; i++) {
        for (int j = i + 1; j < numofPlanes; j++) {
            pthread_rwlock_rdlock(&rwlock);

            // Use global_time to calculate the future positions consistently
           // int delta_i = global_time - shared_data[i].get_time();
           // int delta_j = global_time - shared_data[j].get_time();

            int delta = 1;

            int future_x_of_i = shared_data[i].get_x() + shared_data[i].get_speedX() * delta;
            int future_y_of_i = shared_data[i].get_y() + shared_data[i].get_speedY() * delta;
            int future_z_of_i = shared_data[i].get_z() + shared_data[i].get_speedZ() * delta;

            int future_x_of_j = shared_data[j].get_x() + shared_data[j].get_speedX() * delta;
            int future_y_of_j = shared_data[j].get_y() + shared_data[j].get_speedY() * delta;
            int future_z_of_j = shared_data[j].get_z() + shared_data[j].get_speedZ() * delta;

            int check_x = future_x_of_j - future_x_of_i;
            int check_y = future_y_of_j - future_y_of_i;
            int check_z = future_z_of_j - future_z_of_i;

            int squared_horizontal_distance = check_x * check_x + check_y * check_y;
            int squared_vertical_distance = check_z * check_z;



            if ((squared_horizontal_distance <= min_horizontal) && (squared_vertical_distance <= min_vertical)) {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "Collision detected between plane " << shared_data[i].get_id()
                          << " and plane " << shared_data[j].get_id()
                          << " will occur within " << delta << " second(s)!" << std::endl;
            }

            pthread_rwlock_unlock(&rwlock);
        }
    }
}


//Print function used to test if memory mapping was correct, was just for testing purposes (hence why no critical section protection
void ComputerSystem::printPlanes() {
    if (shared_data == MAP_FAILED) {
        cerr << "Shared memory was not mapped correctly" << endl;
        return;
    }

    if (numofPlanes <= 0) {
        cerr << "No planes were on the file" << endl;
        return;
    }

	for (int i = 0; i < numofPlanes; ++i) {
			cout<< "ID: " << shared_data[i].get_id() << " Time: " << shared_data[i].get_time() << " Position: (" << shared_data[i].get_x() << ", " << shared_data[i].get_y() << ", " << shared_data[i].get_z() << ")"
					  << " Speed: (" << shared_data[i].get_speedX() << ", " << shared_data[i].get_speedY() << ", " << shared_data[i].get_speedZ() << ")"
					  << endl;
		}
}
