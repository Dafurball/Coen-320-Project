
#include "CommunicationSystem.h"
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>
#include <unistd.h>
#include <sys/dispatch.h>
#include <thread>

#include "airplane.h"
#include "planeManager.h"


#define SEPAR "\n\n-------------------------------------------------------------------------------------------------------------------\n\n"
#define ERROR "\n\n/////////////////////////////////////////////////////////////\n\n"

using namespace std;

//Message structure for IPC
typedef struct {
    unsigned int id;
    char command[100];
    int valueX;
    int valueY;
} msg_struct;


///////////////////////////////////////////////////////////////////////////////////////////////

CommunicationSystem::CommunicationSystem(planeManager & manager) : manager(&manager) {}

CommunicationSystem::~CommunicationSystem(){}


//start thread
void CommunicationSystem::startCommunicationThread(){
	running = true;
	pthread_create(&communicationThread, nullptr, startServerThread, this);


}

//get thread
pthread_t CommunicationSystem::getCommunicationThread() const{
		return communicationThread;
}

//Channel between ComputerSytem(client) and CommunicationSystem(server)
//server
void* CommunicationSystem::startServerThread(void* arg) {
    CommunicationSystem* system = static_cast<CommunicationSystem*>(arg);

    //Open Channel
    name_attach_t* attach = name_attach(NULL, "CommunicationSystemServer", 0);
    if (attach == NULL) {
        perror("name_attach");
        return nullptr;
    }

    cout <<  "CommunicationSystem is running, waiting for operator's commands ...";

    while (true) {
        int rcvid;
        msg_struct msg;

        //Message
        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
        if (rcvid == -1) {
            perror("MsgReceive");
            continue;
        }

        // Ignore pulse messages
        if (rcvid == 0) {
            continue;
        }


        //Now based on command from controller we change speed, altitude, position of airplane
        //or we change delta(timeBetweenCollision) or we print airplane info
        string command(msg.command);

        if (command == "cs") {
        	cout <<endl;
            system->manager->changeSpeed(msg.id, msg.valueX);
            cout <<endl;
        } else if (command == "ca") {
        	cout <<endl;
            system->manager->changeAltitude(msg.id, msg.valueX);
            cout <<endl;
        } else if (command == "cp") {
        	cout <<endl;
            system->manager->changeDirection(msg.id, msg.valueX, msg.valueY);
            cout <<endl;
        } else if (command == "ct") {
//            cout << "Do nothing" << std::endl;
        } else if (command == "pp") {
        	cout << SEPAR << "REQUESTING PLANE INFORMATION FROM CONSOLE: \n\n";
            system->manager->printPlane(msg.id);
            cout << SEPAR;
        } else {
        	cout <<endl;
            cout << "Unknown command received: " << command;
            cout <<endl;
        }

        //Reply to client
        msg_struct reply;
        reply.id = msg.id;
        strncpy(reply.command, "\nCommunicationSystem: Command Processed", sizeof(reply.command) - 1);
        reply.command[sizeof(reply.command) - 1] = '\0';
        reply.valueX = 0;
        reply.valueY = 0;

        MsgReply(rcvid, 0, &reply, sizeof(reply));
    }

    // Detach the channel
    name_detach(attach, 0);
    return nullptr;
}

