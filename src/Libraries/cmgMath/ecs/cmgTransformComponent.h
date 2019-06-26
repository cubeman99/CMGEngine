#ifndef _CMG_MATH_TRANSFORM_COMPONENT_H_
#define _CMG_MATH_TRANSFORM_COMPONENT_H_

#include <cmgCore/ecs/cmgECSSystem.h>
#include <cmgMath/types/cmgTransform3f.h>


struct TransformComponent : public ECSComponent<TransformComponent>
{
	union
	{
		Transform3f transform;
		struct
		{
			Vector3f position;
			Quaternion rotation;
			Vector3f scale;
		};
	};

	TransformComponent()
	{
		position = Vector3f::ZERO;
		scale = Vector3f::ONE;
		rotation = Quaternion::IDENTITY;
	}
};


#endif // _CMG_MATH_TRANSFORM_COMPONENT_H_