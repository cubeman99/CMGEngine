#include "Pacejka.h"


// http://www.edy.es/dev/docs/pacejka-94-parameters-explained-a-comprehensive-guide/


//--------------------------------------------------------------------------------------------------------------
// Pacejka ’94 Longitudinal Force parameters:
//--------------------------------------------------------------------------------------------------------------
//  Param	Role															Units		Typical Range	Sample
//--------------------------------------------------------------------------------------------------------------
//	b0		Shape factor																1.4 .. 1.8		1.5
//	b1		Load influence on longitudinal friction coefficient (*1000)		1/kN		-80 .. +80		0
//	b2		Longitudinal friction coefficient (*1000)									900 .. 1700		1100
//	b3		Curvature factor of stiffness/load								N/%/kN^2	-20 .. +20		0
//	b4		Change of stiffness with slip									N/%			100 .. 500		300
//	b5		Change of progressivity of stiffness/load						1/kN		-1 .. +1		0
//	b6		Curvature change with load^2												-0.1 .. +0.1	0
//	b7		Curvature change with load													-1 .. +1		0
//	b8		Curvature factor															-20 .. +1		-2
//	b9		Load influence on horizontal shift								%/kN		-1 .. +1		0
//	b10		Horizontal shift												%			-5 .. +5		0
//	b11		Vertical shift													N			-100 .. +100	0
//	b12		Vertical shift at load = 0										N			-10 .. +10		0
//	b13		Curvature shift																-1 .. +1		0
//--------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------
// Pacejka ’94 longitudinal formula
//--------------------------------------------------------------------------------------------------------------
// Coeff	Name				Parameters			Formula
//--------------------------------------------------------------------------------------------------------------
//	C		Shape factor		b0					C = b0
//	D		Peak factor			b1, b2				D = Fz · (b1·Fz + b2)
//	BCD		Stiffness			b3, b4, b5			BCD = (b3·Fz2 + b4·Fz) · e(-b5·Fz)
//	B		Stiffness factor	BCD, C, D			B = BCD / (C·D)
//	E		Curvature factor	b6, b7, b8, b13		E = (b6·Fz2 + b7·Fz + B8) · (1 – b13·sign(slip+H))
//	H		Horizontal shift	b9, b10				H = b9·Fz + b10
//	V		Vertical shift		b11, b12			V = b11·Fz + b12
//	Bx1		(composite)								Bx1 = B · (slip + H)
//--------------------------------------------------------------------------------------------------------------
//
//	F = D · sin(C · arctan(Bx1 – E · (Bx1 – arctan(Bx1)))) + V
//
//	where:
//
//		F = longitudinal force in N (newtons)
//		Fz = vertical force in kN (kilonewtons)
//		slip = slip ratio in percentage (0..100)
//--------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------
// Initial values for setting up the longitudinal curve:
//--------------------------------------------------------
//	Reference load	4000
//	Maximum load	13000
//	b0				1.5
//	b2				1100
//	b4				300
//	b8				-2
//	all other		0
//---------------------------------------------------------


MagicFormula::MagicFormula()
{
	b[0]	= 0; // Shape factor
	b[1]	= 0; // Load influence on longitudinal friction coefficient (*1000)
	b[2]	= 0; // 
	b[3]	= 0;
	b[4]	= 0;
	b[5]	= 0;
	b[6]	= 0;
	b[7]	= 0;
	b[8]	= 0;
	b[9]	= 0;
	b[10]	= 0;
	b[11]	= 0;
	b[12]	= 0;
	b[13]	= 0;
}

void MagicFormula::Calcluate()
{
	float camber = 0.0f;
	float load = 1.0f;


}


SimplifiedMagicFormula::SimplifiedMagicFormula()
{
}

float SimplifiedMagicFormula::Calcluate(float k)
{
	//float result = d * Math::Sin(c * Math::ATan((b * k * (1 - e)) - (e * Math::ATan(b * k))));
	float result = d * Math::Sin(c * Math::ATan(b*k - e*(b*k - Math::ATan(b * k))));

	return result;
}

