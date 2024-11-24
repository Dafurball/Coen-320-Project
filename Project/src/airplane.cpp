#include <iostream>
#include <cmath>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "airplane.h"
#include "ResourceProtection.h"

using namespace std;

airplane::airplane() {
	// TODO Auto-generated constructor stub

}

airplane::airplane (int time, int id, int x, int y, int z, int SpeedX, int SpeedY, int SpeedZ ){
	this-> time = time;
	this-> id = id;
	this-> x = x;
	this-> y = y;
	this-> z = z;
	this-> SpeedX = SpeedX;
	this-> SpeedY = SpeedY;
	this-> SpeedZ = SpeedZ;
}

airplane::~airplane() {
	// TODO Auto-generated destructor stub
}

int airplane::get_id(){
	return id;
}

int airplane::get_time(){
	return time;
}

int airplane::get_x(){
	return x;
}
int airplane::get_y(){
	return y;
}
int airplane::get_z(){
	return z;
}
int airplane::get_speedX(){
	return SpeedX;
}
int airplane::get_speedY(){
	return SpeedY;
}

int airplane::get_speedZ(){
	return SpeedZ;
}

//start routine for each pthread of type airplane created
//I now see the power of typecasting, I didnt have to change anything when changing implementation from airplane objects to shared memory!
void* airplane::location_update(void *arg){

	//typecasting the void argument passed by pthread_create into one of type airplane
    airplane* plane = static_cast<airplane*>(arg);

    while (true) {
    	sem_wait(&airplane_semaphore);

    	plane->new_location();
    	sleep(1);//plane->print();



    	//time for when location is updated
    }
    return nullptr;

}

//Method that updates an airplane's location based on their current coordinates plus their speed multiplied by the delta (which is 1 since we update every second)
void airplane::new_location(){

    //pthread_mutex_lock(&reader_mutex);
    //sem_wait(&shared_access);
	pthread_rwlock_wrlock(&rwlock);
    x = x + SpeedX * delta;
    y = y + SpeedY * delta;
    z = z + SpeedZ * delta;

    time = time + delta;
	pthread_rwlock_unlock(&rwlock);



  //  sem_post(&shared_access);
    //pthread_mutex_unlock(&reader_mutex);

    //Print was used to test functionality of airplane class
  //  print();

}

//Prints an airplane's info, including their speed (as a whole value, not their x/y/x components).  Mostly for testing
void airplane::print(){
	double speed = sqrt(SpeedX*SpeedX + SpeedY*SpeedY + SpeedZ*SpeedZ);

    std::lock_guard<std::mutex> lock(cout_mutex);

	cout << "The Test from airplane: Plane ID: " << id << " at time "<< time << " seconds has coordinates (" << x << "," << y << "," << z << ") with a speed of "  << speed << endl;
}
