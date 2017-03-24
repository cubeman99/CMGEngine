#ifndef _TACHOMETER_H_
#define _TACHOMETER_H_

#include <driving/Graphics.h>
#include "Speedometer.h"


class Tachometer : public Gauge
{
public:
	Tachometer();


	void Draw(Graphics& g, const Vector2f& position, float radius) override;

	inline void SetGearNumber(unsigned int gearNumber) { m_gearNumber = gearNumber; }

private:
	unsigned int m_gearNumber;
};


#endif // _TACHOMETER_H_