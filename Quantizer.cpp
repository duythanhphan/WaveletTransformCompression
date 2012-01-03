/*
 * Quantizer.cpp
 *
 *  Created on: 02-01-2012
 *      Author: Michal
 */

#include <cmath>

#include "Quantizer.h"

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


