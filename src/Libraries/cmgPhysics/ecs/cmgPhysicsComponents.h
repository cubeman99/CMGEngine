#ifndef _CMG_PHYSICS_ECS_PHYSICS_COMPONENTS_H_
#define _CMG_PHYSICS_ECS_PHYSICS_COMPONENTS_H_

#include <cmgMath/types/cmgTransform3f.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgCore/ecs/cmgECSSystem.h>
#include <cmgPhysics/cmgMotionIntegrators.h>


struct TransformComponent : public ECSComponent<TransformComponent>
{
	Transform3f transform;
};

struct LinearMotionComponent : public ECSComponent<LinearMotionComponent>
{
	Vector3f velocity = Vector3f::ZERO;
	Vector3f acceleration = Vector3f::ZERO;
};

struct AngularMotionComponent : public ECSComponent<AngularMotionComponent>
{
	Vector3f velocity = Vector3f::ZERO;
	Vector3f acceleration = Vector3f::ZERO;
	Vector3f originOffset = Vector3f::ZERO;

	// Cached values
	friend class AngularMotionSystem;
	Matrix4f bodyToWorld = Matrix4f::IDENTITY;
	Vector3f originOffsetWorld = Vector3f::ZERO;
};


class LinearMotionSystem : public BaseECSSystem
{
public:
	LinearMotionSystem() : BaseECSSystem()
	{
		AddComponentType<TransformComponent>();
		AddComponentType<LinearMotionComponent>();
	}

	virtual void UpdateComponents(float delta, BaseECSComponent** components)
	{
		TransformComponent* transform = (TransformComponent*) components[0];
		LinearMotionComponent* motion = (LinearMotionComponent*) components[1];
		integrators::ModifiedEuler(transform->transform.position,
			motion->velocity, motion->acceleration, delta);
	}
};

class AngularMotionSystem : public BaseECSSystem
{
public:
	AngularMotionSystem() : BaseECSSystem()
	{
		AddComponentType<TransformComponent>();
		AddComponentType<AngularMotionComponent>();
	}

	virtual void UpdateComponents(float delta, BaseECSComponent** components)
	{
		TransformComponent* transform = (TransformComponent*) components[0];
		AngularMotionComponent* motion = (AngularMotionComponent*) components[1];

		motion->velocity += motion->acceleration * delta;

		// Calculate the origin's world position
		motion->bodyToWorld.InitRotation(
			transform->transform.rotation);
		motion->bodyToWorld.c3.xyz = transform->transform.position;
		motion->originOffsetWorld = motion->bodyToWorld.TransformAffine(motion->originOffset);

		// First-order angular velocity integration
		Quaternion angularVelocityQuat;
		angularVelocityQuat.xyz = motion->velocity;
		angularVelocityQuat.w = 0.0f;
		transform->transform.rotation += (angularVelocityQuat *
			transform->transform.rotation) * (delta * 0.5f);

		// Rotate the position around the origin
		Vector3f momentArm = transform->transform.position - motion->originOffsetWorld;
		float wLength = motion->velocity.Length();
		if (wLength > 0.0f && momentArm.LengthSquared() > 0.0f)
		{
			Quaternion originRotation = Quaternion(
				motion->velocity / wLength, wLength * delta);
			transform->transform.position -= motion->originOffsetWorld;
			originRotation.RotateVector(transform->transform.position);
			transform->transform.position += motion->originOffsetWorld;
		}

		// Normalize the orientation to keep things in check
		transform->transform.rotation.Normalize();

		// Re-alculate the origin's world position
		motion->bodyToWorld.InitRotation(
			transform->transform.rotation);
		motion->bodyToWorld.c3.xyz = transform->transform.position;
		motion->originOffsetWorld = motion->bodyToWorld.TransformAffine(motion->originOffset);
	}
};


#endif // _CMG_PHYSICS_ECS_PHYSICS_COMPONENTS_H_