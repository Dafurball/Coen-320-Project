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

#include <sys/dispatch.h>
#include "ComputerSystem.h"
#include "radar.h"
#include "airplane.h"
#include "ResourceProtection.h"

using namespace std;

typedef struct {
    unsigned int id;   // Identifier for the message (e.g., Aircraft ID)
    char body[100];    // Content of the message (e.g., command or data)
} msg_struct;

ComputerSystem::ComputerSystem(int numPlanes): numofPlanes(numPlanes), running_collision(false) {
    // Open the shared memory segment created by radar
    shm_fd= shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error opening shared memory for ComputerSystem" << std::endl;
       return;
    }

    //Truncate the shared memory to the correct size (big errors if not included I found out lol)
    if (ftruncate(shm_fd, sizeof(airplane) * numofPlanes) == -1) {
        perror("truncate failed");
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
    running_collision = true;
    pthread_create(&ComputerSystem_thread, nullptr, collision, this);
}

void ComputerSystem::startComms(){
	running_coms = true;
	pthread_create(&comms_thread, nullptr, startServer, this);
}

//Used to join computerSystem pthread in main
pthread_t ComputerSystem::getSystemThread() const {
    return ComputerSystem_thread;
}

pthread_t ComputerSystem::getComThread() const {
    return comms_thread;
}




void* ComputerSystem::startServer(void*) {
	    // 1. Create a connection name for the server
	    name_attach_t* attach = name_attach(NULL, "ComputerSystemServer", 0);
	    if (attach == NULL) {
	        perror("name_attach");
	        nullptr;
	    }

	    std::cout << "ComputerSystem: Server is running, waiting for messages..." << std::endl;

	    // 2. Listen for incoming messages
	    while (true) {
	        int rcvid;
	        msg_struct msg; // Message received from the client
	        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

	        if (rcvid == -1) {
	            perror("MsgReceive");
	            continue;
	        }

	        if (rcvid == 0) {
	           //filter out background noise
	        	continue;
	        }

	        std::cout << "ComputerSystem: Received command for Aircraft " << msg.id
	                  << ": " << msg.body << std::endl;

	        // Simulate processing the command
	        std::string processedMessage = "Processed Command: " + std::string(msg.body);

	        // 3. Reply back to the client
	        msg_struct reply;
	        reply.id = msg.id;
	        strncpy(reply.body, processedMessage.c_str(), sizeof(reply.body) - 1);
	        reply.body[sizeof(reply.body) - 1] = '\0'; // Ensure null termination

	        MsgReply(rcvid, 0, &reply, sizeof(reply));
	    }

	    // 4. Detach the name
	    name_detach(attach, 0);
	    nullptr;
}

//the method our ComputerSystem thread calls repeatedly
void * ComputerSystem::collision(void * arg) {
	ComputerSystem* system = static_cast<ComputerSystem*>(arg);
	    while (system->running_collision) {

	    	sem_wait(&collision_semaphore);

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

            //How far into the future do we check for collisions, need to find a way to change/set it during runtime
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


