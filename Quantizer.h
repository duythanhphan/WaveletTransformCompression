/*
 * Quantizer.h
 *
 *  Created on: 02-01-2012
 *      Author: Michal
 */

#ifndef QUANTIZER_H_
#define QUANTIZER_H_

class Quantizer {
public:
	Quantizer(double min, double max, unsigned int numberOfIntervals);

	double quantize(double value);
	double inverseQuantize(double index);

public:
	static double round(double value);
	static inline double getApproximation(double value, double step);

private:
	double m_dMin;
	double m_dMax;
	unsigned int m_iNumberOfIntervals;
	double m_dIntervalLength;

	double m_dAddValue;//(m_dIntervalLenght / 2) + m_dMin
};

double Quantizer::getApproximation(double value, double step) {
	return step * round(value / step);
}

#endif /* QUANTIZER_H_ */
