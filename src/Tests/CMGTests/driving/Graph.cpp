#include "Graph.h"

Graph::Graph() :
	m_markerPosition(0.0f)
{
	m_lineColor = Color::WHITE;
	m_textColor = Color::WHITE;
	m_axisColor = Color::GRAY;
	m_backgroundColor = Color::BLACK;
	m_outlineColor = Color::WHITE;
	m_markerColor = Color::YELLOW;
	m_mins = -Vector2f::ONE;
	m_maxs = Vector2f::ONE;
}

Graph::Graph(const String& title, const Color& lineColor) :
	m_title(title),
	m_lineColor(lineColor),
	m_markerPosition(0.0f)
{
	m_textColor = Color::WHITE;
	m_axisColor = Color::GRAY;
	m_backgroundColor = Color::BLACK;
	m_outlineColor = Color::WHITE;
	m_markerColor = Color::YELLOW;
	m_mins = -Vector2f::ONE;
	m_maxs = Vector2f::ONE;
}

void Graph::AddPoint(float x, float y)
{
	if (m_points.empty())
	{
		m_mins.Set(x, y);
		m_maxs.Set(x, y);
	}
	else
	{
		m_mins.x = Math::Min(x, m_mins.x);
		m_mins.y = Math::Min(y, m_mins.y);
		m_maxs.x = Math::Max(x, m_maxs.x);
		m_maxs.y = Math::Max(y, m_maxs.y);
	}
	m_points.push_back(Vector2f(x, y));
}

void Graph::Clear()
{
	m_points.clear();
	m_mins = -Vector2f::ONE;
	m_maxs = Vector2f::ONE;
}

void Graph::SetMarkerPosition(float x)
{
	m_markerPosition = x;
}

float Graph::GetInterpolatedValue(float x) const
{
	if (m_points.empty() || x < m_points.front().x || x > m_points.back().x)
		return 0.0f;

	for (unsigned int i = 1; i < m_points.size(); i++)
	{
		if (x < m_points[i].x)
		{
			float x1 = m_points[i - 1].x;
			float y1 = m_points[i - 1].y;
			float x2 = m_points[i].x;
			float y2 = m_points[i].y;
			float t = (x - x1) / (x2 - x1);
			return ((y1 * (1 - t)) + (y2 * t));
		}
	}
	return 0.0f;
}


bool Graph::GetPointInWindow(const Vector2f& graphPoint, Vector2f& outWindowPoint) const
{
	Vector2f maxs = m_maxs;
	Vector2f mins = m_mins;

	if (maxs.x - mins.x < 0.0001f)
	{
		mins.x -= 1;
		maxs.x += 1;
	}
	if (maxs.y - mins.y < 0.0001f)
	{
		mins.y -= 1;
		maxs.y += 1;
	}
	{
		Vector2f size = maxs - mins;
		Vector2f inflation = size * 0.1f;
		mins.y -= inflation.y;
		maxs.y += inflation.y;
	}

	outWindowPoint = ((graphPoint - mins) / (maxs - mins));
	outWindowPoint.y = 1.0f - outWindowPoint.y;

	return (outWindowPoint.x >= 0.0f && outWindowPoint.y >= 0.0f &&
			outWindowPoint.x <= 1.0f && outWindowPoint.y <= 1.0f);
}

