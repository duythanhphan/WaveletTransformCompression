/*
 * Encoder.h
 *
 *  Created on: 06-01-2012
 *      Author: Michal
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <map>

class Encoder {
public:
	Encoder(unsigned int initialSize);
	~Encoder();

	void encode(unsigned int code, unsigned int numberOfBits);

	inline unsigned int* getData();
	inline unsigned int getSize() const;
	inline unsigned int encodedSize() const;

private:
	void allocateAdditionalMemory();
private:
	unsigned int* m_pData;
	unsigned int m_iCurrentSize;
	unsigned int m_iAdditionalMemorySize;

	unsigned int m_iCurrentIndex;
	unsigned int m_iCurrentBitPosition;
	unsigned int m_iMaxBitPosition;

	std::map<unsigned int, unsigned int> m_offMasks;
};

unsigned int* Encoder::getData() {
	return m_pData;
}

unsigned int Encoder::getSize() const {
	return m_iCurrentSize;
}

unsigned int Encoder::encodedSize() const {
	return m_iCurrentIndex + 1;
}

#endif /* ENCODER_H_ */
