
#include "CommunicationSystem.h"
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

using namespace std;

CommunicationSystem::CommunicationSystem()
    : shm_fd(-1), numofPlanes(0), shared_data(nullptr), runningCommunicationSystem(false) {
    // Constructor implementation
    // Open the shared memory segment created by another system (e.g., Radar)
    shm_fd = shm_open("/airplane_data", O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error opening shared memory for CommunicationSystem" << std::endl;
        return;
    }

    // Map the shared memory to the process's address space
    struct stat shm_stat;
    if (fstat(shm_fd, &shm_stat) == -1) {
        std::cerr << "Error getting shared memory size" << std::endl;
        close(shm_fd);
        return;
    }
    numofPlanes = shm_stat.st_size / sizeof(airplane);
    shared_data = static_cast<airplane*>(mmap(nullptr, shm_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shared_data == MAP_FAILED) {
        std::cerr << "Error mapping shared memory in CommunicationSystem" << std::endl;
        close(shm_fd);
        return;
    }
}

CommunicationSystem::~CommunicationSystem() {
	// TODO Auto-generated destructor stub
}

///////////////////////////////////////////////////////////////////////////////////////////////

//Handle sending message to airplane

airplane* CommunicationSystem::findAirplaneID(int id) {
    for (int i = 0; i < numofPlanes; i++) {


        if (shared_data[i].get_id() == id) {
            return &shared_data[i];
        }

    }
    return nullptr;
}

void CommunicationSystem::sendMessageToAirplane(int id, const std::string& command) {

	//getting the airplane with the passed ID
    airplane* airplane = findAircraftByID(id);

    if (airplane) {
        cout << "COMMAND TO airplane with ID: " << id << ": " << command << endl;
        // Assuming the airplane class has a processCommand method
        airplane->processCommand(command);
    } else {
        cerr << "Error: Airplane ID " << id << " does not exist.\n";
    }
}


// Start Thread
void CommunicationSystem::startCommunicationThread() {
    runningCommunicationSystem = true;
    pthread_create(&CommunicationSystem_thread, nullptr, [](void* arg) -> void* {
        CommunicationSystem* comm = static_cast<CommunicationSystem*>(arg);
        while (comm->runningCommunicationSystem) {
            // Simulate repetitive communication tasks (e.g., processing commands)
            sleep(1); // Placeholder for real functionality
        }
        return nullptr;
    }, this);
}





//void CommunicationSystem::startCommunicationThread(){
//	runningCommunicationSystem = true;
//
//	pthread_create(&communication_thread, nullptr, communicationThreadTransmission, this);
//
//}



//static void* communicationThreadTransmission(void* arg) {
//    Communication* comm = static_cast<Communication*>(arg);
//
//    while (comm->running) {
//        std::unique_lock<std::mutex> lock(comm->queue_mutex);
//        // Wait for a new command or stop signal
//        comm->cv.wait(lock, [&]() { return !comm->command_queue.empty() || !comm->running; });
//
//        if (!comm->running) break; // Exit if stopping
//
//        // Retrieve the next command
//        auto [aircraftID, command] = comm->command_queue.front();
//        comm->command_queue.pop();
//        lock.unlock();
//
//        // Process the command
//        airplane* targetAircraft = comm->findAircraftByID(aircraftID);
//        if (targetAircraft) {
//            std::cout << "Sending command to Aircraft " << aircraftID << ": " << command << std::endl;
//            targetAircraft->processCommand(command); // Assuming this function exists
//        } else {
//            std::cerr << "Error: Aircraft " << aircraftID << " not found!" << std::endl;
//        }
//    }
//    return nullptr;
//}





