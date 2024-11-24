/*
 * CommunicationSystem.h
 *
 *  Created on: Nov 8, 2024
 *      Author: 15145
 */

#ifndef SRC_COMMUNICATIONSYSTEM_H_
#define SRC_COMMUNICATIONSYSTEM_H_

#include <string>
#include "airplane.h"

class CommunicationSystem {
public:
	CommunicationSystem();
	virtual ~CommunicationSystem();

	//Valeriia
	//Send command to plane with ID
	void sendMessageToAirplane(int id, const std::string& command);

	airplane* findAirplaneID(int id);


	//
	void startCommunicationThread();       // repetitively called by CommunicationSystem thread

private:
	int shm_fd;		//shared memory descriptor
	int numofPlanes;
	airplane* shared_data;	//pointer to shared data


	bool runningCommunicationSystem;	//to indicate if the Communication System is running

	pthread_t CommunicationSystem_thread;



};

#endif /* SRC_COMMUNICATIONSYSTEM_H_ */
