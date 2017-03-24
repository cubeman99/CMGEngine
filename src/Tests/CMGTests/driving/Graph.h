#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <cmgGraphics/cmg_graphics.h>
#include <cmgMath/cmg_math.h>


class Graph
{
public:
	friend class Graphics;

public:
	Graph();
	Graph(const String& title, const Color& lineColor);

	inline void SetTitle(const String& title) { m_title = title; }
	inline void SetLineColor(const Color& lineColor) { m_lineColor = lineColor; }

	void AddPoint(float x, float y);
	void Clear();

	bool GetPointInWindow(const Vector2f& graphPoint, Vector2f& outWindowPoint) const;

	void SetMarkerPosition(float x);

	float GetInterpolatedValue(float x) const;

private:
	String		m_title;

	Color		m_textColor;
	Color		m_lineColor;
	Color		m_markerColor;
	Color		m_axisColor;
	Color		m_backgroundColor;
	Color		m_outlineColor;
	Vector2f	m_mins;
	Vector2f	m_maxs;
	std::vector<Vector2f> m_points;

	float		m_markerPosition;
};


#endif // _GRAPH_H_