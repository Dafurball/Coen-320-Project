#include <iostream>
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
#include "ResourceProtection.h"
#include "Timer.h"


///////////////////////////////////////////////////////////////////////////////////////Timer Structure and Methods for Set Up//////////////////////////////////////////////////////////////////////////////////
atomic<int> global_time{0};

sem_t airplane_semaphore, radar_semaphore, collision_semaphore;

struct TimerData {
    int numPlanes; // Number of planes
};

void timer_callback(union sigval);

void setup_timer(timer_t& timerid, TimerData* timerData);



int main() {

/////////////////////////////////////////////////////////////////////////////////////////Initializing//////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Load file we will be reading from for airplanes
    const std::string filename = "/data/var/tmp/example.txt";


    int numofPlanes = 0;
    int numofInc = 0;

    int totalInc = 0;
    int totalAir = 0;

    string line;


    airplane* airplanes = nullptr;
    airplane* incoming = nullptr;
    pthread_t* airplane_threads = nullptr;
    airplane* shared_data = nullptr;


/////////////////////////////////////////////////////////////////////////////////////Dynamic Array Setup/////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }


    //Count of total number of planes currently in airspace vs scheduling to be arriving in airspace.  Used for dynamic array creation
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        int time, id, x, y, z, speedX, speedY, speedZ;
        if (iss >> time >> id >> x >> y >> z >> speedX >> speedY >> speedZ) {
        	if ( time > 0){
        		totalInc++;
        	}
        	else if (time == 0){
        		totalAir++;
        	}
        }
    }

    //Create out dynamic arrays
    airplanes = new airplane[totalAir];
    incoming = new airplane[totalInc];
    airplane_threads = new pthread_t[totalAir];

    //Resets file cursor for next step
    inputFile.clear();
    inputFile.seekg(0);


    //Fill the two arrays of currently in airspace vs scheduled to be arriving by going through file
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        int time, id, x, y, z, speedX, speedY, speedZ;
        if (iss >> time >> id >> x >> y >> z >> speedX >> speedY >> speedZ) {
        	if ( time > 0){
        		incoming[numofInc] = airplane(time, id, x, y, x, speedX, speedY, speedZ);
        		numofInc++;

        	}
        	else if (time == 0){
        		airplanes[numofPlanes] = airplane(time, id, x, y, z, speedX, speedY, speedZ);
        		numofPlanes++;
        	}
        }
    }
    inputFile.close();

/////////////////////////////////////////////////////////////////////////////////////Memory Mapping Set up//////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Creating shared memory for airplane data
    int shared_fd = shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (shared_fd == -1) {
        perror("shm_open failed");
        return 1;
    }

    //Resize shared memory to fit number of planes in airspace
    if (ftruncate(shared_fd, sizeof(airplane) * numofPlanes) == -1) {
        perror("ftruncate failed");
        return 1;
    }

    //Memory map the shared memory
    shared_data = static_cast<airplane*>(mmap(nullptr, sizeof(airplane) * numofPlanes, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0));
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    //Copy airplanes in airspace data to shared memory
    for (int i = 0; i < numofPlanes; ++i) {
    		memcpy(&shared_data[i], &airplanes[i], sizeof(airplane));

    }

    // Step 6: Create threads for airplanes
    for (int i = 0; i < numofPlanes; ++i) {
    		pthread_create(&airplane_threads[i], nullptr, airplane::location_update, &shared_data[i]);

    }


////////////////////////////////////////////////////////////////////////////////////Timers and Resource Protection/////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize resource protection
    initializeResourceProtection(numofPlanes);

    //Creating TimerData for the global timer
    TimerData timerData = { numofPlanes };

    //Global timer set up
    timer_t global_timer;
    setup_timer(global_timer, &timerData);


////////////////////////////////////////////////////////////////////////////////////Remaining ATC Set Up////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Initializing radar
    radar Radar(numofPlanes);
    Radar.startRadarThread();

    //Initializing computerSystem
    ComputerSystem system(numofPlanes);
    system.startSystemThread();

    //Joining all the threads into the main thread
    pthread_join(Radar.getRadarThread(), nullptr);
    pthread_join(system.getSystemThread(), nullptr);
    for (int i = 0; i < numofPlanes; ++i) {
        pthread_join(airplane_threads[i], nullptr);
    }

///////////////////////////////////////////////////////////////////////////////////////////////Clean Up///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    munmap(shared_data, sizeof(airplane) * numofPlanes);
    shm_unlink("/airplane_data");
    delete[] airplanes;
    delete[] airplane_threads;

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

