#include "cmgPhysicsMesh.h"


PhysicsMesh::PhysicsMesh() :
	m_body(nullptr)
{
}

PhysicsMesh::PhysicsMesh(RigidBody* body) :
	m_body(body)
{
}

PhysicsMesh::~PhysicsMesh()
{
}


void PhysicsMesh::AddShape(CollisionPrimitive* shape)
{
	shape->body = m_body;
	m_shapes.push_back(shape);
}
	
Vector3f PhysicsMesh::GetCenterOfMass() const
{
	Vector3f centerOfMass = Vector3f::ZERO;
	for (unsigned int i = 0; i < m_shapes.size(); ++i)
		centerOfMass += m_shapes[i]->bodyToShape.GetTranslation() * m_shapes[i]->GetVolume();
	centerOfMass *= 1.0f / GetVolume();
	return centerOfMass;
}

float PhysicsMesh::GetVolume() const
{
	float volume = 0.0f;
	for (unsigned int i = 0; i < m_shapes.size(); ++i)
		volume += m_shapes[i]->GetVolume();
	return volume;
}

void PhysicsMesh::CalcInertiaTensor(float mass, Matrix3f& outInertiaTensor) const
{
	Matrix3f inverseInertiaTensor;
	CalcInertiaTensor(mass, outInertiaTensor, inverseInertiaTensor);
}

void PhysicsMesh::CalcInverseInertiaTensor(float mass, Matrix3f& outInverseInertiaTensor) const
{
	Matrix3f inertiaTensor;
	CalcInertiaTensor(mass, inertiaTensor, outInverseInertiaTensor);
}

void PhysicsMesh::CalcInertiaTensor(float mass,
	Matrix3f& outInertiaTensor, Matrix3f& outInverseInertiaTensor) const
{
	if (mass == 0.0f || m_shapes.empty())
	{
		outInertiaTensor.SetZero();
		outInverseInertiaTensor.SetZero();
		return;
	}

	float volume = GetVolume();

	if (volume == 0.0f)
	{
		outInertiaTensor.SetZero();
		outInverseInertiaTensor.SetZero();
		return;
	}

	float density = mass / volume;

	// Accumulate the inertia from each shape.
	Matrix3f totalInertia = Matrix3f::ZERO;
	for (unsigned int i = 0; i < m_shapes.size(); i++)
	{
		CollisionPrimitive* shape = m_shapes[i];
		float primitiveMass = density * shape->GetVolume();

		// Calculate the local inertia tensor for this shape.
		Matrix3f localInertiaTensor;
		shape->CalculateInertiaTensor(primitiveMass, localInertiaTensor);
		
		// Transform the inertia tensor into mesh space.
		localInertiaTensor = shape->bodyToShape.Get3x3().GetTranspose() * localInertiaTensor;

		// Apply the parrallel axes theorem.
		Vector3f offsetFromCenterOfMass = shape->bodyToShape.GetTranslation();
		totalInertia += localInertiaTensor + Matrix3f::CreateScale(
			offsetFromCenterOfMass * offsetFromCenterOfMass * primitiveMass);
	}

	outInertiaTensor = totalInertia;
	outInverseInertiaTensor = outInertiaTensor;
	outInverseInertiaTensor.Invert();
}

void PhysicsMesh::ShiftOriginToCenterOfMass()
{
	Vector3f centerOfMass = GetCenterOfMass();
	for (unsigned int i = 0; i < m_shapes.size(); ++i)
	{
		m_shapes[i]->bodyToShape.SetTranslation(
			m_shapes[i]->bodyToShape.GetTranslation() - centerOfMass);
	}
}

