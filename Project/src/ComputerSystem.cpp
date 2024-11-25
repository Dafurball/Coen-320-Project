#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <mutex>
#include <queue>

#include <sys/dispatch.h>
#include "ComputerSystem.h"
#include "radar.h"
#include "airplane.h"
#include "ResourceProtection.h"

using namespace std;

#define SEPAR "\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n"

#define SEPARATE "\n\n#####################################################################\n\n"

//Message structure for IPC

typedef struct {
    unsigned int id;
    char command[100];
    int value;
} msg_struct;

int tempId=0;
string tempCommand = " ";
int tempValue = 0;


ComputerSystem::ComputerSystem(int numPlanes): numofPlanes(numPlanes), running_collision(false) {
    // Open the shared memory segment created by radar
    shm_fd= shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error opening shared memory for ComputerSystem" << std::endl;
       return;
    }

    //Truncate the shared memory to the correct size (big errors if not included I found out lol)
    if (ftruncate(shm_fd, sizeof(airplane) * numofPlanes) == -1) {
        perror("truncate failed");
        return;
    }


    // Map the shared memory to this process's address space (read-only)
    shared_data = static_cast<airplane*>(mmap(nullptr, sizeof(airplane) * numofPlanes, PROT_READ, MAP_SHARED, shm_fd, 0));
    if (shared_data == MAP_FAILED) {
        std::cerr << "Error mapping shared memory in ComputerSystem" << std::endl;
        close(shm_fd);
        return;
    }
}

ComputerSystem::~ComputerSystem() {
	// TODO Auto-generated destructor stub
}

void ComputerSystem::startSystemThread() {
    running_collision = true;
    pthread_create(&ComputerSystem_thread, nullptr, collision, this);
}

void ComputerSystem::startComms(){
	running_coms = true;
	pthread_create(&comms_thread, nullptr, startServer, this);
}


//
void ComputerSystem::startCommunicationThread(){
	pthread_create(&toCommunication_thread, nullptr, processCommandsToCommunication, this);
}

//Used to join computerSystem pthread in main
pthread_t ComputerSystem::getSystemThread() const {
    return ComputerSystem_thread;
}

pthread_t ComputerSystem::getComThread() const {
    return comms_thread;
}

pthread_t ComputerSystem::getToCommunication_thread() const{
	return toCommunication_thread;
}




void* ComputerSystem::startServer(void*) {


	    // Attach the server to a channel
	    name_attach_t* attach = name_attach(NULL, "ComputerSystemServer", 0);
	    if (attach == NULL) {
	        perror("name_attach");
	        return nullptr;
	    }

	    cout <<SEPAR << "ComputerSystem: Server is running, waiting for messages..." << std::endl;

	    while (true) {
	        int rcvid;
	        msg_struct msg; // Message received from the client

	        // Receive a message
	        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
	        if (rcvid == -1) {
	            perror("MsgReceive");
	            continue;
	        }

	        if (rcvid == 0) {
	            continue; // Ignore pulse messages
	        }

	        // Log the received message
	        cout<<SEPAR << "ComputerSystem: Received command for Aircraft " << msg.id
	                  << ": " << msg.command << " with value " << msg.value << endl;

//////////////////////////////////////////////////////////////////////////////////////
	        //Block
	        // Passing command to Plane
	        //

	         tempId=msg.id;
	         tempCommand = msg.command;
	         tempValue = msg.value;



	       	//Block
	        //////////////////////////////////////////////////////////////////////////////////////


	        // Reply back to the client (acknowledge the message)
	        msg_struct reply;
	        reply.id = msg.id; // Echo back the ID
	        strncpy(reply.command, ">> ComputerSystem: Received Command", sizeof(reply.command) - 1);
	        reply.command[sizeof(reply.command) - 1] = '\0'; // Ensure null termination
	        reply.value = 0; // Optional value for reply

	        MsgReply(rcvid, 0, &reply, sizeof(reply));

//	        msg_struct reply = {msg.id};
//	        strncpy(reply.command, "Acknowledged", sizeof(reply.command) - 1);
//	        reply.command[sizeof(reply.command) - 1] = '\0';
//	        reply.value = 0; // No processing, just acknowledgment
//
//	        MsgReply(rcvid, 0, &reply, sizeof(reply));
	    }

	    // Detach the channel
	    name_detach(attach, 0);
	    return nullptr;
}

//Third thread for channel betwe computer and communication
void* ComputerSystem::processCommandsToCommunication(void *arg){
//	ComputerSystem* sys = static_cast<ComputerSystem*>(arg);
//	sys->processCommandsToCommunication();

	  // Define the CommunicationSystem channel name
	    const char* CHANNEL_NAME = "CommunicationSystemServer";

	    while (true) {
	        //Getting values of global variables
	        unsigned int localId = tempId;
	        std::string localCommand = tempCommand;
	        int localValue = tempValue;

	        //skip
	        if (localId == 0 || localCommand == " " || localValue == 0) {
	            sleep(1);
	            continue;
	        }

	        //1
	        //Channel open
	        int coid = name_open(CHANNEL_NAME, 0);
	        if (coid == -1) {
	            std::cerr << "ERROR: Failed to connect to channel: " << CHANNEL_NAME << std::endl;
	            sleep(1); // Retry after a delay
	            continue;
	        }

	        cout <<SEPARATE << "ComputerSystem: Connected to CommunicationSystem on channel: " << CHANNEL_NAME << std::endl;

	        // Prepare the message
	        msg_struct msg;
	        msg.id = localId;
	        strncpy(msg.command, localCommand.c_str(), sizeof(msg.command) - 1);
	        msg.command[sizeof(msg.command) - 1] = '\0'; // Ensure null termination
	        msg.value = localValue;

	        // Send the message to CommunicationSystem
	        msg_struct reply;
	        if (MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
	            perror("Error sending message");
	            name_close(coid);
	            sleep(1); // Retry after a delay
	            continue;
	        }

	        // Print the reply from CommunicationSystem
	        cout <<SEPARATE << "ComputerSystem: Received reply from CommunicationSystem - " << reply.command << endl<<SEPARATE;

	        // Close the channel
	        name_close(coid);

	        // Reset global variables after sending
	        tempId = 0;
	        tempCommand = " ";
	        tempValue = 0;

	        // Sleep to avoid rapid consecutive iterations
	        sleep(1);
	    }

	    return nullptr;


}




//the method our ComputerSystem thread calls repeatedly
void * ComputerSystem::collision(void * arg) {
	ComputerSystem* system = static_cast<ComputerSystem*>(arg);
	    while (system->running_collision) {

	    	sem_wait(&collision_semaphore);

            system->collisionTest();

            sleep(1);

	    }
	    return nullptr;
}

//Checks collisions, uses nifty trick for calculating distance without having to use sqrt function
void ComputerSystem::collisionTest() {
    const int min_horizontal = 9000000; // 3000^2
    const int min_vertical = 1000000;   // 1000^2

    for (int i = 0; i < numofPlanes; i++) {
        for (int j = i + 1; j < numofPlanes; j++) {
            pthread_rwlock_rdlock(&rwlock);

            //How far into the future do we check for collisions, need to find a way to change/set it during runtime
            int delta = 1;

            int future_x_of_i = shared_data[i].get_x() + shared_data[i].get_speedX() * delta;
            int future_y_of_i = shared_data[i].get_y() + shared_data[i].get_speedY() * delta;
            int future_z_of_i = shared_data[i].get_z() + shared_data[i].get_speedZ() * delta;

            int future_x_of_j = shared_data[j].get_x() + shared_data[j].get_speedX() * delta;
            int future_y_of_j = shared_data[j].get_y() + shared_data[j].get_speedY() * delta;
            int future_z_of_j = shared_data[j].get_z() + shared_data[j].get_speedZ() * delta;

            int check_x = future_x_of_j - future_x_of_i;
            int check_y = future_y_of_j - future_y_of_i;
            int check_z = future_z_of_j - future_z_of_i;

            int squared_horizontal_distance = check_x * check_x + check_y * check_y;
            int squared_vertical_distance = check_z * check_z;



            if ((squared_horizontal_distance <= min_horizontal) && (squared_vertical_distance <= min_vertical)) {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "\t\t\t\tCollision detected between plane " << shared_data[i].get_id()
                          << " and plane " << shared_data[j].get_id()
                          << " will occur within " << delta << " second(s)!" << std::endl;
            }

            pthread_rwlock_unlock(&rwlock);
        }
    }
}



//with old message struct
//void* ComputerSystem::startServer(void*) {
//
//	    //1
//		//Create a connection name for the server
//	    name_attach_t* attach = name_attach(NULL, "ComputerSystemServer", 0);
//	    if (attach == NULL) {
//	        perror("name_attach");
//	        nullptr;
//	    }
//
//	    cout <<SEPAR<< "ComputerSystem: Server is running, waiting for messages...\n\n";
//
//
//	    //2
//	    //Listen for incoming messages
//	    while (true) {
//	        int rcvid;
//	        msg_struct msg; // Message received from the client
//	        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
//
//	        if (rcvid == -1) {
//	            perror("MsgReceive");
//	            continue;
//	        }
//
//	        //
//	        //Filtering the noise!!!!!
//	        if (rcvid == 0) {
//	           //filter out background noise
//	        	continue;
//	        }
//
//	        //printout received on server side
//	        cout <<SEPAR<< "ComputerSystem: Received command for Airplane " << msg.id
//	                  << ": " << msg.body << endl<<SEPAR;
//
//
//	    //3
//	        //Reply back to the client
//	        msg_struct reply;
//	        reply.id = msg.id;
//	        strncpy(reply.body, processedMessage.c_str(), sizeof(reply.body) - 1);
//	        reply.body[sizeof(reply.body) - 1] = '\0'; // Ensure null termination
//
//	        MsgReply(rcvid, 0, &reply, sizeof(reply));
//	    }
//
//	    // 4. Detach the name
//	    name_detach(attach, 0);
//	    nullptr;
//}




