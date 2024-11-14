/*
 * main.cpp
 *
 *  Created on: Nov. 12, 2024
 *      Author: dafur
 */

#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mutex.h"
#include "airplane.h"
#include "radar.h"

int main(){



radar Radar("example.txt");

Radar.startRadarThread();






	return 0;
}
