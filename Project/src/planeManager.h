

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

    int checkIds(int);

    void printPlane(int);
    void changeSpeed(int, int);
    void changeAltitude(int,int);
    void changeDirection(int,int,int);

private:
    const std::string filename;
    int numOfPlanes;
    airplane* planes;
    pthread_t* planeThreads;
    airplane* shared_data;
    int sharedFd;
};

#endif /* SRC_PLANEMANAGER_H_ */
