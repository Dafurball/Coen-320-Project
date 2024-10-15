
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <mutex>

#include "airplane.h"

using namespace std;

mutex m;

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




void* airplane::location_update(void *arg){
    airplane* plane = static_cast<airplane*>(arg);
    while (true) {
        m.lock();
    	plane->new_location();
    	m.unlock();
        sleep(1);
    }
    return nullptr;

}

void airplane::new_location(){
    int delta = 1;

    x = x + SpeedX * delta;
    y = y + SpeedY * delta;
    z = z + SpeedZ * delta;

    print();

}
void airplane::print(){
	double speed = sqrt(SpeedX*SpeedX + SpeedY*SpeedY + SpeedZ*SpeedZ);

	cout << "Plane ID: " << id << " at time "<< time << " minute has coordinates (" << x << "," << y << "," << z << ") with a speed of "  << speed << endl;
}
