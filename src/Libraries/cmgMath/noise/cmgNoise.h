#ifndef _NOISE_H_
#define _NOISE_H_

#include <cmgMath/types/cmgVector2f.h>


namespace NoiseLib
{
	float PerlinNoise(float x, float y);
	float PerlinNoise(float x, float y, float z);
	float PerlinNoise(const Vector2f& v);
	Vector2f Gradient(int x, int y);

	
	float Fade(float t);
	float Lerp(float t, float a, float b);
	float Grad(int hash, float x, float y, float z);
};


#endif // _NOISE_H_


