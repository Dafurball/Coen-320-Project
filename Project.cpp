#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "airplane.h"

using namespace std;

int main() {

	airplane* plane = new airplane(1,001,3,4,5,6,7,8);
	airplane* plane2 = new airplane(2,002,6,8,10,12,14,16);

	pthread_t airplane_thread, airplane_thread2;

sleep(2);

	pthread_create(&airplane_thread, nullptr, airplane::location_update, plane);
	pthread_create(&airplane_thread2, nullptr, airplane::location_update, plane2);


	sleep(10);
	return 0;
}
