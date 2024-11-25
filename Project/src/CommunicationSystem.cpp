
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

void CommunicationSystem::startCommunicationThread(){
	running = true;
	pthread_create(&serverThreadHandle, nullptr, serverThread, this);



}
pthread_t CommunicationSystem::getCommunicationThread() const{
		return serverThread;
}










