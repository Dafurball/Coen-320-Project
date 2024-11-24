

#ifndef SRC_PLANEMANAGER_H_
#define SRC_PLANEMANAGER_H_

#include "airplane.h"
#include <string>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class planeManager {
public:
    planeManager(const std::string& filename);
    ~planeManager();

    void initialize();
    void startPlaneThreads();
    void stopPlaneThreads();

    int getNumOfPlanes() const;
    airplane* getSharedData() const;

private:
    const std::string filename;
    int numOfPlanes;
    airplane* planes;
    pthread_t* planeThreads;
    airplane* sharedData;
    int sharedFd;
};

#endif /* SRC_PLANEMANAGER_H_ */
