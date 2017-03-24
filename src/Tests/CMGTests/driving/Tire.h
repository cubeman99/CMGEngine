#ifndef _TIRE_H_
#define _TIRE_H_

#include <cmgMath/cmg_math.h>
#include "Pacejka.h"

class Car;



class Tire
{
public:
	enum 
	{
		k_front_right,
		k_front_left,
		k_rear_left,
		k_rear_right,
	};

public:
	Tire();

	Vector3f GetWorldPosition() const;

	void Reset();
	void CalcDerivedData();
	void SetWeightLoad(float weight);
	void AddTransmissionTorque(float torque);
	Vector3f CalculateForce(float timeDelta);
	void ClearAccumulators();
	

	// Tire properties
	Car*					m_car;
	Vector3f				m_offset; // Positional offset from the center of the car.
	float					m_radius;
	bool					m_isDrivenWheel;
	float					m_inertia;

	// Tire model
	SimplifiedMagicFormula	m_pacejkaLateral;
	SimplifiedMagicFormula	m_pacejkaLongitudinal;
	float					m_pacejkaMinSpeed;

	// Dynamic state
	float		m_weight;			// The weight loaded on this tire.
	float		m_steeringAngle;	// positive = counter-clockwise.
	float		m_angularVelocity;	// Rotational speed in radians per second.
	float		m_torque;			// Torque applied to this tire.
	float		m_rotationAngle;

public:
	// Transformation matrices
	Matrix4f	m_tireToCar;
	Matrix4f	m_carToTire;
	Matrix4f	m_tireToWorld;
	Matrix4f	m_worldToTire;

	// Derived data
	float		m_slipAngle;
	float		m_slipRatio;
	float		m_longitudinalGrip;
	float		m_lateralGrip;
	float		m_longitudinalForce;
	float		m_lateralForce;
};


#endif // _TIRE_H_