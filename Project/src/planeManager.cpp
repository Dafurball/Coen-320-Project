#include "PlaneManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include "ResourceProtection.h"


planeManager::planeManager(const std::string& filename)
    : filename(filename), numOfPlanes(0), planes(nullptr), planeThreads(nullptr), shared_data(nullptr), sharedFd(-1) {}

planeManager::~planeManager() {
    stopPlaneThreads();

    if (shared_data) {
        munmap(shared_data, sizeof(airplane) * numOfPlanes);
    }

    if (sharedFd != -1) {
        shm_unlink("/airplane_data");
    }

    delete[] planes;
    delete[] planeThreads;
}
////////////////////////////////////////////////////////////////////////////////Shared Memory Set up///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void planeManager::initialize() {

    //Load file we will be reading from for airplanes
    ifstream inputFile(filename);
    if (!inputFile) {
        throw std::runtime_error("Failed to open airplane data file: " + filename);
    }

    // First, count the number of planes
    numOfPlanes = 0;
    std::string line;
    while (std::getline(inputFile, line)) {
        numOfPlanes++;
    }

    // Allocate dynamic arrays
    planes = new airplane[numOfPlanes];
    planeThreads = new pthread_t[numOfPlanes];

    //Resets file cursor for next step
    inputFile.clear();
    inputFile.seekg(0);

    //Count of total number of planes, used for dynamic array creation
    int index = 0;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        int time, id, x, y, z, speedX, speedY, speedZ;
        if (iss >> time >> id >> x >> y >> z >> speedX >> speedY >> speedZ) {
            planes[index] = airplane(time, id, x, y, z, speedX, speedY, speedZ);
            index++;
        }
    }
    inputFile.close();

    //Creating shared memory for airplane data
    sharedFd = shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (sharedFd == -1) {
        throw std::runtime_error("Failed to create shared memory");
    }

    //Resize shared memory to fit number of planes in airspace
    if (ftruncate(sharedFd, sizeof(airplane) * numOfPlanes) == -1) {
        throw std::runtime_error("Failed to resize shared memoory");
    }

    //Memory map the shared memory
	shared_data = static_cast<airplane*>(mmap(nullptr, sizeof(airplane) * numOfPlanes,
											 PROT_READ | PROT_WRITE, MAP_SHARED, sharedFd, 0));
	if (shared_data == MAP_FAILED) {
		throw std::runtime_error("Failed to map shared memory");
	}

	//Copy airplanes in airspace data to shared memory
	for (int i = 0; i < numOfPlanes; ++i) {
		memcpy(&shared_data[i], &planes[i], sizeof(airplane));
	}
}

//Creates threads for airplane
void planeManager::startPlaneThreads() {
	for (int i = 0; i < numOfPlanes; ++i) {
		if (pthread_create(&planeThreads[i], nullptr, airplane::location_update, &shared_data[i]) != 0) {
			throw std::runtime_error("Failed to create plane thread");
		}
	}
}

void planeManager::stopPlaneThreads() {
	for (int i = 0; i < numOfPlanes; ++i) {
		if (planeThreads[i]) {
			pthread_join(planeThreads[i], nullptr);
		}
	}
}

///////////////////////////////////////////////////////////Used by radar and computersystem to set up shared memory//////////////////////////////////////////////////////////////////////////////////////
int planeManager::getNumOfPlanes() const {
	return numOfPlanes;
}

airplane* planeManager::getSharedData() const {
	return shared_data;
}

/////////////////////////////////////////////////////////////////////////////////////Methods used by Communications//////////////////////////////////////////////////////////////////////////////////////
    int planeManager::checkIds(int id){
		pthread_rwlock_rdlock(&rwlock);

    	for (int i = 0; i < numOfPlanes; i++){

    		if (id == shared_data[i].get_id()){
    			pthread_rwlock_unlock(&rwlock);

    			return 1;
    		}
    	}
		pthread_rwlock_unlock(&rwlock);

    	return 0;
    }

    void planeManager::printPlane(int id){
    	pthread_rwlock_rdlock(&rwlock);

		for (int i = 0; i < numOfPlanes; i++){

			if (id == shared_data[i].get_id()){
		        std::lock_guard<std::mutex> lock(cout_mutex);

				cout << "Plane " << shared_data[i].get_id() << " has altitude of " << shared_data[i].get_z() << " in the direction of (" << shared_data[i].get_x() << "," << shared_data[i].get_y() << ") with a speed of " <<
						 shared_data[i].get_speed() << endl;

				pthread_rwlock_unlock(&rwlock);

				break;
			}
		}
		pthread_rwlock_unlock(&rwlock);


    }

void planeManager::changeSpeed(int id, int newSpeed){
	pthread_rwlock_rdlock(&rwlock);

	for (int i = 0; i < numOfPlanes; i++){

		if (id == shared_data[i].get_id()){

			double currentSpeed = shared_data[i].get_speed();

			double speedScaling = newSpeed/currentSpeed;

			shared_data[i].change_speed(speedScaling);

	        std::lock_guard<std::mutex> lock(cout_mutex);

			cout << "Plane " << id << " will now have a speed of " << newSpeed << endl;

			pthread_rwlock_unlock(&rwlock);

			return;
		}
	}
	pthread_rwlock_unlock(&rwlock);
	return;
}

void planeManager::changeAltitude(int id,int altitude){
	pthread_rwlock_rdlock(&rwlock);

	for (int i = 0; i < numOfPlanes; i++){

		if (id == shared_data[i].get_id()){

			shared_data[i].change_altitude(altitude);

			std::lock_guard<std::mutex> lock(cout_mutex);
			cout << "Plane " << id << " will now have an altitude of " << altitude << endl;

			pthread_rwlock_unlock(&rwlock);

			return;
		}
	}
	pthread_rwlock_unlock(&rwlock);
	return;

}

void planeManager::changeDirection(int id ,int x,int y){
	pthread_rwlock_rdlock(&rwlock);

	for (int i = 0; i < numOfPlanes; i++){

		if (id == shared_data[i].get_id()){

			shared_data[i].change_direction(x,y);

			std::lock_guard<std::mutex> lock(cout_mutex);

			cout << "Plane " << id << " will now go in the direction of (" << x << ","<< y <<")" << endl;

			pthread_rwlock_unlock(&rwlock);

			return;
		}
	}
	pthread_rwlock_unlock(&rwlock);
	return;
}
