#include "OperatorConsole.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

#include "OperatorConsole.h"

OperatorConsole::OperatorConsole() {
	    // Open shared memory created by Radar
	    shm_fd = shm_open("/airplane_data", O_RDWR, 0666);
	    if (shm_fd == -1) {
	        cerr << "Error opening shared memory for OperatorConsole" << endl;
	        exit(EXIT_FAILURE);
	    }

	    // Map shared memory into this process's address space
	    shared_data = static_cast<airplane*>(mmap(nullptr, sizeof(airplane) * numPlanes, PROT_READ | PROT_WRITE, shm_fd, 0));
	    if (shared_data == MAP_FAILED) {
	        cerr << "Error mapping shared memory in OperatorConsole" << endl;
	        close(shm_fd);
	        exit(EXIT_FAILURE);
	    }

	    // Open or create semaphore for synchronizing console actions
	    console_semaphore = sem_open("/console_semaphore", O_CREAT, 0666, 1);
	    if (console_semaphore == SEM_FAILED) {
	        cerr << "Error creating semaphore for OperatorConsole" << endl;
	        munmap(shared_data, sizeof(airplane) * numPlanes);
	        close(shm_fd);
	        exit(EXIT_FAILURE);
	    }


}

OperatorConsole::~OperatorConsole() {
	// TODO Auto-generated destructor stub
}

