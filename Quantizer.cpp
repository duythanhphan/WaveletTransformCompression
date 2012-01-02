/*
 * Quantizer.cpp
 *
 *  Created on: 02-01-2012
 *      Author: Michal
 */

#include <cmath>

#include "Quantizer.h"

Quantizer::Quantizer(double min, double max, unsigned int numberOfValues, double startValue) :
		m_iNumberOfValues(numberOfValues), m_dMin(min), m_dMax(max) {

	m_dStepSize = (m_dMax - m_dMin) / (double)m_iNumberOfValues;
	m_pValues = new double[m_iNumberOfValues];

	initValues(startValue);
}

Quantizer::Quantizer(double min, double max, double stepSize, double startValue) :
		m_dStepSize(stepSize), m_dMin(min), m_dMax(max) {

	m_iNumberOfValues = (int)ceil(((m_dMax - m_dMin) / m_dStepSize));
	m_pValues = new double[m_iNumberOfValues];
	initValues(startValue);
}

void Quantizer::initValues(double startValue) {
	double value = startValue;
	for(int i = 0; i < m_iNumberOfValues; ++i) {
		m_pValues[i] = value;
		value += m_dStepSize;
	}
}

Quantizer::~Quantizer() {
	if(m_pValues != 0) {
		delete[] m_pValues;
		m_pValues = 0;
	}
}

/*
 * 		min + stepSize * steps = q
 * 		steps = (q - min) / stepSize
 * 		index = (int)steps
 */
double Quantizer::getApproximation(double value) {
	int index = (int)((value - m_dMin) / m_dStepSize);

	if(index < 0) {
		index = 0;
	} else if(index > m_iNumberOfValues - 1) {
		index = m_iNumberOfValues - 1;
	}

	return m_pValues[index];
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
