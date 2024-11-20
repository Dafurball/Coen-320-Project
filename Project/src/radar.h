/*
 * radar.h
 *
 *  Created on: Nov. 12, 2024
 *      Author: dafur
 */

#ifndef RADAR_H_
#define RADAR_H_

#include <string>
#include <pthread.h>
#include <mutex>

#include "airplane.h"


using namespace std;

class radar {
public:
	radar();
    radar(const string& filename);
	virtual ~radar();

	void loadPlanes(const string& filename);
	//void setupSharedMemory();
//	void copyDataToSharedMemory();
	void printPlanes();
	//airplane* getAirplanes();
	int getnumofPlanes();

	//void startAirplaneThreads();
	void stopAirplaneThreads();
	void startRadarThread();
	pthread_t getRadarThread() const;
	void stopRadarThread();

	static void* updater(void* arg);


private:
	airplane* airplanes;
	int numofPlanes;
    int shared_fd;
    airplane* shared_data;

    pthread_t *airplane_threads;
    pthread_t radar_thread;

    bool running;
    mutex mtx;

};

#endif /* RADAR_H_ */
