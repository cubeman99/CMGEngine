#ifndef _TEST_APP_H_
#define _TEST_APP_H_

#include <cmgApplication/cmg_application.h>
#include <cmgMath/cmg_math.h>
#include <cmgPhysics/cmg_physics.h>


struct Particle;


struct Connection
{
	float distance;
	Particle* particle;

	Connection(Particle* particle, float distance) :
		particle(particle), distance(distance)
	{}
};

struct CircleContour
{
	Vector2f startPoint;
	Vector2f endPoint;
	bool fullCircle;

	CircleContour() :
		fullCircle(false)
	{}

	CircleContour(const Vector2f& startPoint, const Vector2f& endPoint) :
		startPoint(startPoint),
		endPoint(endPoint),
		fullCircle(false)
	{}

	CircleContour(bool fullCircle) :
		fullCircle(fullCircle)
	{}
};

struct Bounds2D
{
	Vector2f center;
	float radius;

	bool CircleContains(const Vector2f& point) const
	{
		return (point.DistToSqr(center) < radius * radius);
	}
	
	bool SquareContains(const Vector2f& point) const
	{
		return (Math::Abs(point.x - center.x) < radius &&
				Math::Abs(point.y - center.y) < radius);
	}
	
	bool SquareContainsSquare(const Bounds2D& bounds) const
	{
		return (bounds.center.x - bounds.radius >= center.x - radius &&
				bounds.center.x + bounds.radius <= center.x + radius &&
				bounds.center.y - bounds.radius >= center.y - radius &&
				bounds.center.y + bounds.radius <= center.y + radius);
	}
	
	bool SquareIntersectsSquare(const Bounds2D& bounds) const
	{
		return (center.x + radius > bounds.center.x - bounds.radius &&
				bounds.center.x + bounds.radius > center.x - radius &&
				center.y + radius > bounds.center.y - bounds.radius &&
				bounds.center.y + bounds.radius > center.y - radius);
	}
};

struct Particle
{
	float m_mass;
	Vector3f m_position;
	Vector3f m_velocity;
	Vector3f m_acceleration;
	Vector3f m_netForce;
	Array<Connection> m_connections;

	Particle();

	void Update(float timeDelta);
};


class TestApp : public Application
{
public:
	TestApp();
	
	void OnInitialize() override;
	void OnQuit() override;
	void OnResizeWindow(int width, int height) override;
	void OnDropFile(const String& fileName) override;
	void OnDropText(const String& text) override;
	void OnUpdate(float timeDelta) override;
	void OnRender() override;


private:

	Vector2f ComputeIntersectionPoint(const Bounds2D& circle, int edgeIndex, int pointIndex, Vector2f* corners);
	void DrawArc(Vector2f a, Vector2f b, const Bounds2D& circle);
	int PerformContour(const Bounds2D& circle, const Bounds2D& square, CircleContour* contours, int& outNumContours);


	RenderParams m_renderParams;


	Transform3f m_modelTransform;

	Transform3f m_cameraTransform;
	Matrix4f m_cameraProjection;
	Texture* m_texture;

	Array<Particle*> m_particles;

	Bounds2D m_square;
	Bounds2D m_circle;
};


#endif // _TEST_APP_H_