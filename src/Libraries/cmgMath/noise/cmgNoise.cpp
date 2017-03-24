#include "cmgNoise.h"
#include <cmgMath/cmgMathLib.h>
#include <cmgMath/noise/cmgNoiseData.h>


float NoiseLib::PerlinNoise(float x, float y)
{
	return PerlinNoise(Vector2f(x, y));
}

float NoiseLib::PerlinNoise(const Vector2f& p)
{
	int x0 = (int) floor(p.x);
	int y0 = (int) floor(p.y);
	int x1 = (int) ceil(p.x);
	int y1 = (int) ceil(p.y);

	//  s---t
	//  |   |
	//  u---v
	
	float s = Vector2f::Dot(Gradient(x0, y0), p - Vector2f((float) x0, (float) y0));
	float t = Vector2f::Dot(Gradient(x1, y0), p - Vector2f((float) x1, (float) y0));
	float u = Vector2f::Dot(Gradient(x0, y1), p - Vector2f((float) x0, (float) y1));
	float v = Vector2f::Dot(Gradient(x1, y1), p - Vector2f((float) x1, (float) y1));

	float Sx = (3 * (p.x - x0) * (p.x - x0)) - (2 * (p.x - x0) * (p.x - x0) * (p.x - x0));
	float a = s + Sx * (t - s);
	float b = u + Sx * (v - u);

	float Sy = (3 * (p.y - y0) * (p.y - y0)) - (2 * (p.y - y0) * (p.y - y0) * (p.y - y0));
	float z = a + Sy * (b - a);

	return z;
}

Vector2f NoiseLib::Gradient(int x, int y)
{
	const int gridWidth  = 10;
	const int gridHeight = 10;
	static bool isInit = false;
	static Vector2f gradients[gridWidth][gridHeight];

	if (!isInit)
	{
		srand(352456);

		for (y = 0; y < gridHeight; y++)
		{
			for (x = 0; x < gridWidth; x++)
			{
				gradients[x][y].Set((float) rand() / RAND_MAX, (float) rand() / RAND_MAX);
				gradients[x][y].Normalize();
				//gradient[y][x] = some_function(r1, r2);
			}
		}
	}

	return gradients[x % gridWidth][y % gridHeight];
}





float NoiseLib::PerlinNoise(float x, float y, float z)
{
	int* p = perm_a;

	// Find the unit cube that contains the point
	int X = (int) floor(x) & 255;
	int Y = (int) floor(y) & 255;
	int Z = (int) floor(z) & 255;

	// Find relative x, y, z of point in cube
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	// Compute fade curves for each of x, y, z
	float u = Fade(x);
	float v = Fade(y);
	float w = Fade(z);

	// Hash coordinates of the 8 cube corners
	int A = p[X] + Y;
	int AA = p[A] + Z;
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;

	// Add blended results from 8 corners of cube
	float res = Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z), Grad(p[BA], x-1, y, z)), Lerp(u, Grad(p[AB], x, y-1, z), Grad(p[BB], x-1, y-1, z))),	Lerp(v, Lerp(u, Grad(p[AA+1], x, y, z-1), Grad(p[BA+1], x-1, y, z-1)), Lerp(u, Grad(p[AB+1], x, y-1, z-1),	Grad(p[BB+1], x-1, y-1, z-1))));
	return (res + 1.0f) / 2.0f; // Convert from range [-1, 1] to [0, 1].
}


float NoiseLib::Fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float NoiseLib::Lerp(float t, float a, float b)
{
	return a + t * (b - a); 
}

float NoiseLib::Grad(int hash, float x, float y, float z)
{
	int h = hash & 15;
	// Convert lower 4 bits of hash inot 12 gradient directions
	float u = h < 8 ? x : y,
			v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

