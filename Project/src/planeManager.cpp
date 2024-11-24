#include "PlaneManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>

planeManager::planeManager(const std::string& filename)
    : filename(filename), numOfPlanes(0), planes(nullptr), planeThreads(nullptr), sharedData(nullptr), sharedFd(-1) {}

planeManager::~planeManager() {
    stopPlaneThreads();

    if (sharedData) {
        munmap(sharedData, sizeof(airplane) * numOfPlanes);
    }

    if (sharedFd != -1) {
        shm_unlink("/airplane_data");
    }

    delete[] planes;
    delete[] planeThreads;
}

void planeManager::initialize() {
    std::ifstream inputFile(filename);
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

    // Reset file cursor and parse the plane data
    inputFile.clear();
    inputFile.seekg(0);

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

    // Set up shared memory
    sharedFd = shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (sharedFd == -1) {
        throw std::runtime_error("Failed to create shared memory");
    }

    // Resize shared memory
    if (ftruncate(sharedFd, sizeof(airplane) * numOfPlanes) == -1) {
        throw std::runtime_error("Failed to resize shared memoory");
    }

        // Map shared memory
        sharedData = static_cast<airplane*>(mmap(nullptr, sizeof(airplane) * numOfPlanes,
                                                 PROT_READ | PROT_WRITE, MAP_SHARED, sharedFd, 0));
        if (sharedData == MAP_FAILED) {
            throw std::runtime_error("Failed to map shared memory");
        }

        // Copy planes into shared memory
        for (int i = 0; i < numOfPlanes; ++i) {
            memcpy(&sharedData[i], &planes[i], sizeof(airplane));
        }
    }

    void planeManager::startPlaneThreads() {
        for (int i = 0; i < numOfPlanes; ++i) {
            if (pthread_create(&planeThreads[i], nullptr, airplane::location_update, &sharedData[i]) != 0) {
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

    int planeManager::getNumOfPlanes() const {
        return numOfPlanes;
    }

    airplane* planeManager::getSharedData() const {
        return sharedData;
    }

    int planeManager::checkIds(int id){
    	for (int i = 0; i < numOfPlanes; i++){
    		if (id == sharedData[i].get_id()){
    			return 1;
    		}
    	}

    	return 0;
    }
