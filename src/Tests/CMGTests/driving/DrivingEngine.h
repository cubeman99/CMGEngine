#ifndef _DRIVING_ENGINE_H_
#define _DRIVING_ENGINE_H_

#include <cmgApplication/cmg_application.h>
#include <cmgMath/cmg_math.h>
#include "Car.h"
#include "Graphics.h"
#include "Pacejka.h"
#include "TestMeshes.h"
#include "diagrams/Speedometer.h"
#include "diagrams/Tachometer.h"

class Camera
{
public:
	Camera() :
		m_position(Vector3f::ZERO),
		m_orientation(Quaternion::IDENTITY),
		m_fieldOfView(1.6f),
		m_aspectRatio(1.0f),
		m_nearPlane(0.1f),
		m_farPlane(100.0f)
	{}

	Matrix4f GetViewProjection() const
	{
		return GetProjectionMatrix() * GetViewMatrix();
	}

	Matrix4f GetViewMatrix() const
	{
		return Matrix4f::CreateRotation(m_orientation.GetConjugate()) *
			Matrix4f::CreateTranslation(-m_position - (m_orientation.GetBack() * m_distance));
	}
	
	Matrix4f GetProjectionMatrix() const
	{
		return Matrix4f::CreatePerspective(m_fieldOfView,
			m_aspectRatio, m_nearPlane, m_farPlane);
	}

public:
	Vector3f	m_position;
	Quaternion	m_orientation;
	float		m_fieldOfView;
	float		m_aspectRatio;
	float		m_nearPlane;
	float		m_farPlane;
	float		m_distance;
};


class DrivingApp : public Application
{
public:
	DrivingApp();
	
	void OnInitialize() override;
	void OnQuit() override;
	void OnResizeWindow(int width, int height) override;
	void OnUpdate(float timeDelta) override;
	void OnRender() override;

	void DrawWireFrameBox(const Vector3f& halfSize, const Color& color);
	void DrawCar(Car* car);
	void DrawTire(Car* car, unsigned int tireIndex);
	void DrawWheel(float radius, float width, float angle, const Color& color);
	void DrawGridFloor(const Vector3f& center, float squareSize, float gridRadius);

	void DrawFreeBodyDiagram(Car* car, const Vector2f& center, float scale);
	void DrawTractionCircle(const Tire& tire, const Vector2f& center, float radius);
	void DrawMesh(Mesh* mesh, const Color& color, const Matrix4f& modelMatrix);

private:
	Graphics		m_graphics;

	// Simulation state
	unsigned int	m_frameIndex;
	bool			m_isPaused;
	float			m_simulationSpeed;

	// Objects
	Car				m_car;

	// Camera
	Camera			m_camera;
	bool			m_enableCameraFollow;

	// Resources
	Joystick*		m_joystick;
	Mesh*			m_meshWheel;
	Mesh*			m_meshCube;
	Mesh*			m_meshCarChassis;
	SpriteFont*		m_font;
	Shader*			m_shader;

	// Trails
	Trail			m_trailTires[4];
	Trail			m_trailCenter;
	Array<Trail*>	m_trails;

	// Diagrams
	Graph			m_testGraph;
	Graph			m_graphLateralGrip[4];
	Graph			m_graphLongitudinalGrip[4];
	Graph			m_graphSpeed;
	Graph			m_graphLongForce;
	Speedometer		m_speedometer;
	Tachometer		m_tachometer;
};


#endif // _DRIVING_ENGINE_H_