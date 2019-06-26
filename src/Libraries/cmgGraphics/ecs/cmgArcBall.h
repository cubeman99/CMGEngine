#ifndef _CMG_GRAPHICS_ECS_ARC_BALL_H_
#define _CMG_GRAPHICS_ECS_ARC_BALL_H_

#include <cmgCore/ecs/cmgECSComponent.h>
#include <cmgMath/types/cmgVector3f.h>


struct ArcBall : public ECSComponent<ArcBall>
{
	float distance = 1.0f;
	float sensitivity = 0.005f;
	Vector3f center = Vector3f::ZERO;


	inline const Vector3f& GetFocus()
	{
		return center;
	}

	inline void SetFocus(const Vector3f& center)
	{
		this->center = center;
	}

	inline void SetDistance(float distance)
	{
		this->distance = distance;
	}

	inline void SetSensitivity(float sensitivity)
	{
		this->sensitivity = sensitivity;
	}
};


#endif // _CMG_GRAPHICS_ECS_ARC_BALL_H_