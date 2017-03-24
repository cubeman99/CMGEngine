#ifndef _PACEJKA_H_
#define _PACEJKA_H_

#include <cmgMath/cmg_math.h>



class Pacejka
{
public:
	Pacejka();

private:
};


// Pacejka '94.
class MagicFormula
{
public:
	MagicFormula();

	void Calcluate();

private:
	float b[14];
};


// Simplified Pacejka's Magic Formula
class SimplifiedMagicFormula
{
public:
	SimplifiedMagicFormula();

	float Calcluate(float k);

public:
	float b; // Stiffness
	float c; // Shape
	float d; // Peak Coefficient
	float e; // Curvature
};




#endif // _PACEJKA_H_