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

atomic<int> global_time{0};

sem_t airplane_semaphore, radar_semaphore, collision_semaphore;

struct TimerData {
    int numPlanes; // Number of planes
};

void timer_callback(union sigval);

void setup_timer(timer_t& timerid, TimerData* timerData) {
    struct sigevent sev;
    struct itimerspec its;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_callback;
    sev.sigev_value.sival_ptr = timerData; // Pass the timerData to the callback
    sev.sigev_notify_attributes = nullptr;

    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create");
        exit(1);
    }

    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, nullptr) == -1) {
        perror("timer_settime");
        exit(1);
    }
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

int main() {
    // Initialize resource protection (global semaphores, locks, etc.)
    const std::string filename = "/data/var/tmp/example.txt";

    // Step 1: Initialize variables
    int numofPlanes = 0;
    airplane* airplanes = nullptr;
    pthread_t* airplane_threads = nullptr;
    airplane* shared_data = nullptr;

    // Step 2: Open the file and count the number of planes
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        if (!line.empty()) numofPlanes++;
    }

    // Step 3: Allocate memory for airplanes and threads
    airplanes = new airplane[numofPlanes];
    airplane_threads = new pthread_t[numofPlanes];

    // Step 4: Reset file cursor and read airplane data
    inputFile.clear();
    inputFile.seekg(0);

    int index = 0;
    while (std::getline(inputFile, line) && index < numofPlanes) {
        std::istringstream iss(line);
        int time, id, x, y, z, speedX, speedY, speedZ;
        if (iss >> time >> id >> x >> y >> z >> speedX >> speedY >> speedZ) {
            airplanes[index] = airplane(time, id, x, y, z, speedX, speedY, speedZ);
            index++;
        }
    }
    inputFile.close();

    // Step 5: Create shared memory for airplane data
    int shared_fd = shm_open("/airplane_data", O_CREAT | O_RDWR, 0666);
    if (shared_fd == -1) {
        perror("shm_open failed");
        return 1;
    }

    // Resize shared memory to fit airplanes
    if (ftruncate(shared_fd, sizeof(airplane) * numofPlanes) == -1) {
        perror("ftruncate failed");
        return 1;
    }

    // Memory map the shared memory
    shared_data = static_cast<airplane*>(mmap(nullptr, sizeof(airplane) * numofPlanes, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0));
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    // Copy airplane data to shared memory
    for (int i = 0; i < numofPlanes; ++i) {
        memcpy(&shared_data[i], &airplanes[i], sizeof(airplane));
    }

    // Step 6: Create threads for airplanes
    for (int i = 0; i < numofPlanes; ++i) {
        pthread_create(&airplane_threads[i], nullptr, airplane::location_update, &shared_data[i]);
    }

    // Initialize resource protection
    initializeResourceProtection(numofPlanes);

    // Step 7: Create TimerData for the global timer
    TimerData timerData = { numofPlanes };

    // Step 8: Set up the global timer
    timer_t global_timer;
    setup_timer(global_timer, &timerData);

    // Step 9: Create Radar and ComputerSystem
    radar Radar(numofPlanes);
    Radar.startRadarThread();

    ComputerSystem system(numofPlanes);
    system.startSystemThread();

    // Step 10: Join threads in the main thread
    pthread_join(Radar.getRadarThread(), nullptr);
    pthread_join(system.getSystemThread(), nullptr);
    for (int i = 0; i < numofPlanes; ++i) {
        pthread_join(airplane_threads[i], nullptr);
    }

    // Step 11: Cleanup resources
    munmap(shared_data, sizeof(airplane) * numofPlanes);
    shm_unlink("/airplane_data");
    delete[] airplanes;
    delete[] airplane_threads;

    return 0;
}


