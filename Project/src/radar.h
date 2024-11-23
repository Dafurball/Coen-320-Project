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
    radar(int);
	virtual ~radar();

	void loadPlanes(const string& filename);
	void printPlanes();
	int getnumofPlanes();
	void stopAirplaneThreads();
	void startRadarThread();
	pthread_t getRadarThread() const;
	void stopRadarThread();
	static void* updater(void* arg);


private:
	//airplane* airplanes;
	int numofPlanes;
    pthread_t radar_thread;

    int shared_fd;
    airplane* shared_data;




    bool running;

};

#endif /* RADAR_H_ */
