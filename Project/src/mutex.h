/*
 * mutex.h
 *
 *  Created on: Nov. 7, 2024
 *      Author: dafur
 */

//globally declaring this mutex for use within printer and airplane threads
#ifndef MUTEX_H
#define MUTEX_H

#include <mutex>

extern std::mutex m;

#endif // MUTEX_H
