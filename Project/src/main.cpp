#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "airplane.h"
#include "radar.h"
#include "ComputerSystem.h"
#include "OperatorConsole.h"
#include "ResourceProtection.h"
#include "planeManager.h"
#include "CommunicationSystem.h"


///////////////////////////////////////////////////////////////////////////////////////Timer Structure and Methods for Set Up//////////////////////////////////////////////////////////////////////////////////
atomic<int> global_time{0};

sem_t airplane_semaphore, radar_semaphore, collision_semaphore;

struct TimerData {
    int numPlanes; // Number of planes
};

void timer_callback(union sigval);

void setup_timer(timer_t& timerid, TimerData* timerData);

void printWelcome();

int main() {

	//Welcome Message
	printWelcome();

////////////////////////////////////////////////////////////////////////////////////Airplanes and their Manager////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::string filename = "/data/var/tmp/example.txt";

    // Create and initialize the plane manager
    planeManager manager(filename);
    manager.initialize();

    // Start plane threads
    manager.startPlaneThreads();

////////////////////////////////////////////////////////////////////////////////////Timers and Resource Protection/////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize resource protection
    initializeResourceProtection(manager.getNumOfPlanes());

    //Creating TimerData for the global timer
    TimerData timerData = { manager.getNumOfPlanes() };

    //Global timer set up
    timer_t global_timer;
    setup_timer(global_timer, &timerData);


////////////////////////////////////////////////////////////////////////////////////Remaining ATC Set Up////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Start plane threads
    manager.startPlaneThreads();

    //Initialize and start radar threads
    radar Radar(manager.getNumOfPlanes());
    Radar.startRadarThread();

    //Initializing computerSystem
    ComputerSystem system(manager.getNumOfPlanes());
    system.startSystemThread();
    system.startComms();
    system.startCommunicationThread();

    //Initializing Operator
    OperatorConsole console(manager);
    console.startOperatorConsoleThread();

    CommunicationSystem communsystem(manager);
    communsystem.startCommunicationThread();

//    manager.printPlane(103);
//    manager.changeSpeed(103, 100);
//   manager.changeAltitude(103, 100);
//   manager.changeDirection(103, 100, 200);



    //Joining all the threads into the main thread
    pthread_join(Radar.getRadarThread(), nullptr);
    pthread_join(system.getSystemThread(), nullptr);
    pthread_join(system.getComThread(), nullptr);
    pthread_join(console.getconsoleThread(), nullptr);


///////////////////////////////////////////////////////////////////////////////////////////////Clean Up///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    shm_unlink("/airplane_data");

    cleanupSharedResources();

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////Timer Methods///////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup_timer(timer_t& timerid, TimerData* timerData) {
    struct sigevent sev;
    struct itimerspec its;

    //Structure of out sigevent, it will be a thread that used the data passed through timerData in conjuction with the method callback "timer_callback"
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_callback;
    sev.sigev_value.sival_ptr = timerData; // Pass the timerData to the callback
    sev.sigev_notify_attributes = nullptr;

    //Creating our timer, using a real time clock
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    //The timer will fire one second relative to the current time, and will repeat every second
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;

    //Setting up the timer
    timer_settime(timerid, 0, &its, nullptr);
}

void timer_callback(union sigval sv) {
    TimerData* data = static_cast<TimerData*>(sv.sival_ptr);
    int numPlanes = data->numPlanes; // Access the number of planes

    static int tick_count = 0;
    tick_count++;
    global_time++;


    for (int i = 0; i < numPlanes; ++i) {
        sem_post(&airplane_semaphore); // Post once for each airplane thread
    }

    sem_post(&collision_semaphore); // Unblock collision detection
//
    if (tick_count % 5 == 0) {
        sem_post(&radar_semaphore); // Unblock radar every 5 seconds
    }
}


void printWelcome(){

    cout << "************************************************************" << endl;
    cout << "*                                                          *" << endl;
    cout << "*              AIR TRAFFIC CONTROL SYSTEM                  *" << endl;
    cout << "*                                                          *" << endl;
    cout << "************************************************************" << endl;
    cout << "************************************************************" << endl;
    cout << "*                                                          *" << endl;
    cout << "*    Commands:                                             *" << endl;
    cout << "*      pp 1 - Print plane details                          *" << endl;
    cout << "*      ca [value x] - Change altitude                      *" << endl;
    cout << "*      cs [value x]- Change speed                          *" << endl;
    cout << "*      cp [value x] [value y] - Change position            *" << endl;
    cout << "*      ct [delta i]- Change time between collision         *" << endl;
    cout << "*                                                          *" << endl;
    cout << "************************************************************" << endl;
	cout << endl<< endl;

}

