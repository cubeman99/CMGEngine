#include "Speedometer.h"
#include <cmgMath/cmg_math.h>
#include <sstream>


Gauge::Gauge()
{
	m_spindleValue = 0.0f;

	m_minValue = 0;
	m_maxValue = 140;
	m_majorTicks = 20;
	m_minorTicks = 10;
	m_angle = 240 * Math::DEG_TO_RAD;

	m_backgroundColor = Color::BLACK;
	m_borderColor = Color::WHITE;
	m_spindleColor = Color::RED;
	m_textColor = Color::WHITE;
	m_redLineTextColor = Color::RED;

	m_redLineValue = -1;
}


void Gauge::SetSpindleValue(float speed)
{
	m_spindleValue = speed;
}

void Gauge::SetRedLineValue(int redLineValue)
{
	m_redLineValue = redLineValue;
}

void Gauge::Draw(Graphics& g, const Vector2f& position, float radius)
{
	// Draw background.
	g.FillCircle(position, radius, m_backgroundColor, 30);
	
	float majorTickLength = 0.2f;
	float minorTickLength = 0.13f;

	float startAngle = ((3.0f / 2.0f) * Math::PI) - ((Math::TWO_PI - m_angle) * 0.5f);

	// Draw tick marks
	for (int i = m_minValue; i <= m_maxValue; i++)
	{
		float tickLength = -1;
		Color tickColor = m_borderColor;
		float tickWidth = 1.0f;

		if (m_majorTicks != 0 && i % m_majorTicks == 0)
		{
			tickWidth = 2.0f;
			tickLength = majorTickLength;
		}
		else if (m_minorTicks != 0 && i % m_minorTicks == 0)
		{
			tickWidth = 1.0f;
			tickLength = minorTickLength;
		}

		if (m_redLineValue >= 0.0f && i >= m_redLineValue)
		{
			tickColor = m_redLineTextColor;
		}

		if (tickLength > 0.0f)
		{
			float angle = startAngle - ((i / (float) m_maxValue) * m_angle);

			Vector2f endPoint(Math::Cos(angle), -Math::Sin(angle));
			Vector2f p1 = (endPoint * radius) + position;
			Vector2f p2 = p1 - (endPoint * radius * tickLength);

			g.DrawLine(p1, p2, tickColor, tickWidth);
		}
	}
	
	// Draw major tick mark numbers.
	for (int i = m_minValue; i <= m_maxValue; i += m_majorTicks)
	{
		Color tickTextColor = m_textColor;

		float angle = startAngle - ((i / (float) m_maxValue) * m_angle);

		Vector2f endPoint(Math::Cos(angle), -Math::Sin(angle));
		Vector2f p1 = (endPoint * radius) + position;
		Vector2f textPos = p1 - (endPoint * radius * majorTickLength * 2.0f);
		
		if (m_redLineValue >= 0.0f && i >= m_redLineValue)
		{
			tickTextColor = m_redLineTextColor;
		}

		std::stringstream str;
		str << i;
		textPos.x = (float) ((int) textPos.x);
		textPos.y = (float) ((int) textPos.y);
		g.DrawString(str.str(), textPos, tickTextColor, TextAlign::k_centered);
	}
	
	// Draw spindle.
	float spindleLength = 0.9f;
	float spindleAngle = startAngle - ((m_spindleValue / (float) m_maxValue) * m_angle);
	Vector2f spindlePoint(Math::Cos(spindleAngle), -Math::Sin(spindleAngle));
	spindlePoint = (spindlePoint * radius * spindleLength) + position;
	g.DrawLine(position, spindlePoint, m_spindleColor, 2.0f);

	// Draw outline.
	g.DrawCircle(position, radius, m_borderColor, 30, 2.0f);

}


Speedometer::Speedometer() :
	Gauge()
{
	m_minValue = 0;
	m_maxValue = 140;
	m_majorTicks = 20;
	m_minorTicks = 10;
	m_angle = 240 * Math::DEG_TO_RAD;

	m_backgroundColor = Color::BLACK;
	m_borderColor = Color::WHITE;
	m_spindleColor = Color::RED;
	m_textColor = Color::WHITE;
	m_redLineTextColor = Color::RED;
}

void Speedometer::Draw(Graphics& g, const Vector2f& position, float radius)
{
	// Draw base gauge.
	Gauge::Draw(g, position, radius);

	// Draw MPH text.
	Vector2f textPos = position + Vector2f(0, radius * 0.6f);
	Color mphLabelColor(80, 80, 80);
	g.DrawString("MPH", textPos, mphLabelColor, TextAlign::k_centered);
}


