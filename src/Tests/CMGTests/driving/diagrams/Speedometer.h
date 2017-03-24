#ifndef _SPEEDOMETER_H_
#define _SPEEDOMETER_H_

#include <driving/Graphics.h>


class Gauge
{
public:
	Gauge();

	void SetSpindleValue(float speed);
	void SetRedLineValue(int redLineValue);
	virtual void Draw(Graphics& g, const Vector2f& position, float radius);

protected:
	float m_spindleValue;

	int m_minValue;
	int m_maxValue;
	int m_majorTicks;
	int m_minorTicks;
	float m_angle;
	int m_redLineValue;

	Color m_backgroundColor;
	Color m_borderColor;
	Color m_spindleColor;
	Color m_textColor;
	Color m_redLineTextColor;
};


class Speedometer : public Gauge
{
public:
	Speedometer();

	void Draw(Graphics& g, const Vector2f& position, float radius) override;

private:

};


#endif // _SPEEDOMETER_H_