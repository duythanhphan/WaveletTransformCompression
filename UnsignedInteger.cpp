/*
 * UnsignedInteger.cpp
 *
 *  Created on: 30-12-2011
 *      Author: Michal
 */

#include "UnsignedInteger.h"

const unsigned int UnsignedInteger::NUMBER_OF_BITS = 8 * sizeof(unsigned int);

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

unsigned int UnsignedInteger::getClosestPowerOfTwo(unsigned int Integer) {
	unsigned int lastSetBit = 0;
	unsigned int setBitsCounter = 0;
	unsigned int mask = 1;

	for(unsigned int i = 0; i < NUMBER_OF_BITS; ++i) {
		if((Integer & mask) != 0) {
			lastSetBit = i;
			++setBitsCounter;
		}
		mask <<= 1;
	}

	if(setBitsCounter == 1) {
		return Integer;
	}

	if(lastSetBit == 0) {
		return 1;
	}

	return 1 << (lastSetBit + 1);
}
