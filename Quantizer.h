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
	static double round(double value);
	static inline double getApproximation(double value, double step);
};

double Quantizer::getApproximation(double value, double step) {
	return step * round(value / step);
}

#endif /* QUANTIZER_H_ */
