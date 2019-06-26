#ifndef _CMG_GRAPHICS_ECS_ARC_BALL_CONTROL_SYSTEM_H_
#define _CMG_GRAPHICS_ECS_ARC_BALL_CONTROL_SYSTEM_H_

#include <cmgCore/ecs/cmgECSSystem.h>
#include <cmgGraphics/ecs/cmgArcBall.h>
#include <cmgMath/ecs/cmgTransformComponent.h>
#include <cmgInput/cmgMouse.h>


class ArcBallControlSystem : public BaseECSSystem
{
public:
	ArcBallControlSystem();
	
	void SetMouse(Mouse* mouse);
	void SetUpAxis(const Vector3f upAxis);

	virtual void UpdateComponents(float delta, BaseECSComponent** components);

private:
	Mouse* m_mouse;
	Vector3f m_upAxis;
};


#endif // _CMG_GRAPHICS_ECS_ARC_BALL_CONTROL_SYSTEM_H_