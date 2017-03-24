#include "Tachometer.h"


Tachometer::Tachometer() :
	Gauge()
{
	m_gearNumber = 1;

	m_minValue = 0;
	m_maxValue = 10;
	m_majorTicks = 1;
	m_minorTicks = 10;
	m_angle = 240 * Math::DEG_TO_RAD;

	m_backgroundColor = Color::BLACK;
	m_borderColor = Color::WHITE;
	m_spindleColor = Color::RED;
	m_textColor = Color::WHITE;
}

void Tachometer::Draw(Graphics& g, const Vector2f& position, float radius)
{
	// Draw base gauge.
	Gauge::Draw(g, position, radius);

	// Draw RPM text.
	Vector2f textPos = position + Vector2f(0, radius * 0.3f);
	Color mphLabelColor(80, 80, 80);
	g.DrawString("RPM", textPos, mphLabelColor, TextAlign::k_centered);
	textPos.y += radius * 0.15f;
	g.DrawString("x 1000", textPos, mphLabelColor, TextAlign::k_centered);
	
	// Draw gear number text.
	std::string numberNames[] =
	{
		"1st Gear",
		"2nd Gear",
		"3rd Gear",
		"4th Gear",
		"5th Gear",
		"6th Gear",
		"7th Gear",
	};
	textPos += Vector2f(0, radius * 0.25f);
	g.DrawString(numberNames[m_gearNumber - 1], textPos, Color::WHITE, TextAlign::k_centered);
}


