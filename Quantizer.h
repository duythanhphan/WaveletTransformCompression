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
	Quantizer(double min, double max, unsigned int numberOfValues, double startValue);
	Quantizer(double min, double max, double stepSize, double startValue);
	~Quantizer();

	double getApproximation(double value);

public:
	static double round(double value);

private:
	void initValues(double startValue);

private:
	double* m_pValues;
	int m_iNumberOfValues;

	double m_dStepSize;
	double m_dMin;
	double m_dMax;
};

#endif /* QUANTIZER_H_ */
