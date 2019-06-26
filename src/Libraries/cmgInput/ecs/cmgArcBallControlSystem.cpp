#include "cmgArcBallControlSystem.h"
#include <cmgMath/cmgMathLib.h>


ArcBallControlSystem::ArcBallControlSystem()
	: BaseECSSystem()
	, m_mouse(nullptr)
	, m_upAxis(Vector3f::UNITY)
{
	AddComponentType<TransformComponent>();
	AddComponentType<ArcBall>();
}
void ArcBallControlSystem::SetMouse(Mouse* mouse)
{
	m_mouse = mouse;
}

void ArcBallControlSystem::SetUpAxis(const Vector3f upAxis)
{
	m_upAxis = upAxis;
}

void ArcBallControlSystem::UpdateComponents(float delta, BaseECSComponent** components)
{
	TransformComponent* transform = (TransformComponent*) components[0];
	ArcBall* control = (ArcBall*) components[1];
	
	MouseState currState = m_mouse->GetMouseState();
	MouseState prevState = m_mouse->GetPrevMouseState();


	//float camMoveSpeed = 2.0f;
	/*if (keyboard->IsKeyDown(Keys::w))
		m_camera.m_position += m_camera.m_orientation.GetForward() * camMoveSpeed * dt;
	if (keyboard->IsKeyDown(Keys::s))
		m_camera.m_position += m_camera.m_orientation.GetBack() * camMoveSpeed * dt;
	if (keyboard->IsKeyDown(Keys::a))
		m_camera.m_position += m_camera.m_orientation.GetLeft() * camMoveSpeed * dt;
	if (keyboard->IsKeyDown(Keys::d))
		m_camera.m_position += m_camera.m_orientation.GetRight() * camMoveSpeed * dt;
	if (keyboard->IsKeyDown(Keys::e))
		m_camera.m_position += Vector3f::UNITY * camMoveSpeed * dt;
	if (keyboard->IsKeyDown(Keys::q))
		m_camera.m_position -= Vector3f::UNITY * camMoveSpeed * dt;*/

	// Right mouse: rotate camera
	if (m_mouse->IsButtonDown(MouseButtons::right))
	{
		Vector2f mouseDelta(
			(float) (currState.x - prevState.x),
			(float) (currState.y - prevState.y));
		transform->transform.rotation.Rotate(
			m_upAxis, -mouseDelta.x * control->sensitivity);
		transform->transform.rotation.Rotate(
			transform->transform.rotation.GetLeft(),
			mouseDelta.y * control->sensitivity);
	}

	int scroll = currState.z - prevState.z;
	if (scroll != 0)
		control->distance *= Math::Pow(0.8f, (float) scroll);

	transform->transform.position = control->center +
		transform->transform.rotation.GetBack() * control->distance;
}
