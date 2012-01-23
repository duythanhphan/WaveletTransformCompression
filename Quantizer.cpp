/*
 * Quantizer.cpp
 *
 *  Created on: 02-01-2012
 *      Author: Michal
 */

#include <cmath>

#include "Quantizer.h"

Quantizer::Quantizer(double min, double max, unsigned int numberOfIntervals) :
	m_dMin(min), m_dMax(max), m_iNumberOfIntervals(numberOfIntervals),
	m_dIntervalLength((m_dMax - m_dMin) / (double)m_iNumberOfIntervals),
	m_dAddValue((m_dIntervalLength / 2.0) + m_dMin),
	Q((double)m_iNumberOfIntervals){ }

double Quantizer::quantize(double value) {
	//return floor((value - m_dMin) / m_dIntervalLength);
	return (int)(value / Q);
}

double Quantizer::inverseQuantize(double index) {
	return index * Q;
}

double Quantizer::round(double value) {
	double intPart = 0.0;
	double fractionalPart = 0.0;
	fractionalPart = modf(value, &intPart);

	if(value >= 0.0) {
		if(fractionalPart <= 0.5) {
			return intPart;
		} else {
			return intPart + 1.0;
		}
	} else {
		if(fractionalPart <= -0.5) {
			return intPart - 1.0;
		} else {
			return intPart;
		}
	}
}


