/*
 * UnsignedInteger.h
 *
 *  Created on: 30-12-2011
 *      Author: Michal
 */

#ifndef UNSIGNEDINTEGER_H_
#define UNSIGNEDINTEGER_H_

class UnsignedInteger {
public:
	static const unsigned int NUMBER_OF_BITS;
	static const unsigned int LEFT_BIT;
	static const unsigned int RIGHT_BIT;

public:
	static bool isPowerOfTwo(unsigned int Integer);
	static unsigned int getClosestPowerOfTwo(unsigned int Integer);

	static inline void setBitFromLeft(unsigned int* toSet, unsigned int position);
	static inline void setBitFromRight(unsigned int* toSet, unsigned int position);
	unsigned int reverse(unsigned int toReverse);
};

void UnsignedInteger::setBitFromLeft(unsigned int* toSet, unsigned int position) {

}

void UnsignedInteger::setBitFromRight(unsigned int* toSet, unsigned int position) {

}

#endif /* UNSIGNEDINTEGER_H_ */
