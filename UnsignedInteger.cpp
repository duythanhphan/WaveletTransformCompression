/*
 * UnsignedInteger.cpp
 *
 *  Created on: 30-12-2011
 *      Author: Michal
 */

#include "UnsignedInteger.h"

const unsigned int NUMBER_OF_BITS = 8 * sizeof(unsigned int);

bool UnsignedInteger::isPowerOfTwo(unsigned int Integer) {
	unsigned int setBitsCounter = 0;
	unsigned int mask = 1;

	for(unsigned int i = 0; i < NUMBER_OF_BITS; ++i) {
		if((Integer & mask) != 0) {
			++setBitsCounter;
		}
		mask <<= 1;
	}

	return setBitsCounter == 1;
}
