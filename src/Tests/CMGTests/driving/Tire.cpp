#include "Tire.h"
#include "Car.h"

using namespace std;


Tire::Tire()
{
	m_weight = 0.0f;
	m_steeringAngle = 0.0f;
	m_rotationAngle = 0.0f;
	m_angularVelocity = 0.0f;
	m_torque = 0.0f;
	m_inertia = 1.0f;

	m_pacejkaMinSpeed = 0.6f;
	m_pacejkaLongitudinal.b = 10.0f;
	m_pacejkaLongitudinal.c = 1.6f;
	m_pacejkaLongitudinal.d = 1.0f;
	m_pacejkaLongitudinal.e = 0.8f;
	//m_pacejkaLongitudinal.b = 18.0f;
	//m_pacejkaLongitudinal.c = 2.0f;
	//m_pacejkaLongitudinal.d = 1.0f;
	//m_pacejkaLongitudinal.e = 0.8f;
	//m_pacejkaLateral.b = 9.0f;
	//m_pacejkaLateral.c = 1.8f;
	//m_pacejkaLateral.d = 1.0f;
	//m_pacejkaLateral.e = 0.8f;
	m_pacejkaLateral.b = 4.0f;
	m_pacejkaLateral.c = 2.0f;
	m_pacejkaLateral.d = 1.0f;
	m_pacejkaLateral.e = -0.1f;
}

Vector3f Tire::GetWorldPosition() const
{
	Vector3f worldPos = Vector3f::ZERO;
	worldPos = m_tireToCar.TransformAffine(worldPos);
	worldPos = m_car->m_carToWorld.TransformAffine(worldPos);
	return worldPos;
}

void Tire::Reset()
{
	m_weight = 0.0f;
	m_steeringAngle = 0.0f;
	m_rotationAngle = 0.0f;
	m_angularVelocity = 0.0f;
	m_torque = 0.0f;

	m_slipRatio = 1.0f;
	m_slipAngle = 0.0f;
	m_longitudinalGrip = 0.0f;
	m_lateralGrip = 0.0f;
	m_longitudinalForce = 0.0f;
	m_lateralForce = 0.0f;
}

void Tire::CalcDerivedData()
{
	// Transformation matrices.
	m_tireToCar = 
		Matrix4f::CreateTranslation(m_offset) *
		Matrix4f::CreateRotation(Vector3f::UNITY, m_steeringAngle);
	m_carToTire = m_tireToCar;
	m_carToTire.InvertAffine();
	m_tireToWorld = m_car->m_carToWorld * m_tireToCar;
	m_worldToTire = m_tireToWorld;
	m_worldToTire.InvertAffine();
	
	/*
	// Velocity of the tire contact point in world space.
	Vector3f posInWorldSpace = m_tireToWorld.TransformAffine(m_offset);
	Vector3f contactVelocity = m_car->GetVelocityAtPoint(posInWorldSpace);
	//contactVelocity -= (m_angularVelocity * m_radius) * forwardAxis;

	// Slip angle.
	m_slipAngle = 0.0f;

	// Get the tire's velocity in tire space.
	Vector3f relativeVelocity = m_car->m_velocity + m_car->m_angularVelocity.Cross(m_offset);
	relativeVelocity = m_worldToTire.Rotate(relativeVelocity);

	float velLat = relativeVelocity.x;
	float velLong = -relativeVelocity.z;
	
	// Calculate slip angle.
	m_slipAngle = 0.0f;
	if (Math::Abs(velLong) > 0.001f)
	{
		m_slipAngle = Math::ATan2(velLat, velLong);
	}

	// Calculate wheel slip ratio.
	//float wheelAngularVelocity = Math::Abs(m_angularVelocity);
	//float carAngularVelocity = m_car->m_velocity.Dot(m_car->m_orientation.GetForward()) / m_radius;
	//float maxAgularVelocity = Math::Max(wheelAngularVelocity, carAngularVelocity);
	//m_wheelSlip = 0.0f;
	//if (maxAgularVelocity > 0.0001f)
		//m_wheelSlip = (wheelAngularVelocity - carAngularVelocity) / maxAgularVelocity;

	//Vector3f contactPatchVelocity = -forwardAxis * m_angularVelocity * m_radius;
	//Vector3f wheelHubVelocity = m_car->GetVelocityAtPoint(posInWorldSpace);
	//contactPatchVelocity += wheelHubVelocity;

	//float sa = Math::ATan2(contactPatchVelocity.x, contactPatchVelocity.z - wheelHubVelocity.z);


	float tirePatchSpeed = m_angularVelocity * m_radius;
	float carSpeed = m_car->m_velocity.Dot(forwardAxis);
	m_wheelSlip = 1.0f;
	if (carSpeed > 0.0001f)
		m_wheelSlip = (tirePatchSpeed - carSpeed) / carSpeed;
	
	m_slipRatio = 1.0f;
	if (carSpeed > 0.0001f)
		m_slipRatio = (tirePatchSpeed - carSpeed) / carSpeed;

	// Calculate grip.
	float peakSlip = 10.0f * Math::DEG_TO_RAD;
	float peakCoef = 1.0f;

	m_longitudinalGrip = Math::Clamp(m_slipRatio * (peakCoef / peakSlip), -peakCoef, peakCoef);
	m_lateralGrip = 0.0f;
	//m_longitudinalGrip = (2.0f * peakCoef * peakSlip * m_wheelSlip) /
		//((peakSlip * peakSlip) + (m_wheelSlip * m_wheelSlip));


	//-------------------------------------------------------------------------
	// Cornering.

	float turnRadius = 0.0f;
	if (Math::Abs(m_car->m_steeringAngle) > 0.01f && m_weight > 0.0f)
	{

		// Determine the radius of the turn relative to the point between
		// the two rear wheels. This radius will be negative for a negative
		// steering angle.
		float turnRadiusRearCenter = -m_car->m_wheelBase / Math::Tan(m_car->m_steeringAngle);

		Vector3f turnCenter(turnRadiusRearCenter, 0.0f, m_car->m_wheelBase * 0.5f);
		Vector3f pointOnTurnCircle = m_offset - turnCenter;
		float turnRadius = pointOnTurnCircle.Length();
		
		// Determine the centripital acceleration of the car in the direction tangent to the turn circle.
		//float angleOffsetFromCenter = Math::ATan(m_wheelBase / (2.0f * turnRadiusCenter));
		float speedInTurn = contactVelocity.Dot(forwardAxis);

		// Determine the adjusted steering angles for the two front tires
		// for proper Ackerman steering.
		//float turnRadiusRearLeft = turnRadiusRearCenter - (m_wheelTrack * 0.5f);
		//float turnRadiusRearRight = turnRadiusRearCenter + (m_wheelTrack * 0.5f);
		//m_tires[Tire::k_front_left].m_steeringAngle = Math::ATan(m_wheelBase / turnRadiusRearLeft);
		//m_tires[Tire::k_front_right].m_steeringAngle = Math::ATan(m_wheelBase / turnRadiusRearRight);

		//// Determine the turn radius from the center of the car (Pythagorean theorem).
		//float turnRadius = Math::Sign(turnRadiusRearCenter) *
		//	Math::Sqrt((turnRadiusRearCenter * turnRadiusRearCenter) + (m_wheelBase * m_wheelBase) * 0.25f);
		//
		//// Determine the velocity of the car in the direction tangent to the turn circle.
		//float angleOffsetFromCenter = Math::ATan(m_wheelBase / (2.0f * turnRadiusCenter));
		//float speedInTurn = speed / Math::Cos(angleOffsetFromCenter);

		//// Calculate the maximum speed in the turn before slipping occurs.
		//float maxSpeedForTurn = (m_tireFrictionCoefficient * verticalForce * Math::Abs(turnRadiusCenter)) / m_mass;
		//maxSpeedForTurn = Math::Sqrt(maxSpeedForTurn);

		// Apply centripetal acceleration and angular velocity about the car's center.
		//speed = m_velocity.Length();
		float centripetalForce = ((speedInTurn * speedInTurn) / turnRadius);
		Vector3f cf = (pointOnTurnCircle / turnRadius) * -centripetalForce;
		cf = m_car->m_carToWorld.Rotate(cf);
		m_centripetalForce = cf * m_weight / m_car->m_gravity;
		m_lateralForce = centripetalForce * -Math::Sign(pointOnTurnCircle.x) * m_weight / m_car->m_gravity;

		//m_velocity += m_orientation.GetLeft() * centripetalAcceleration * timeDelta;
		//m_angularVelocity.y = speedInTurn / turnRadiusCenter;
		//m_velocity.Normalize();
		//m_velocity *= speed;
		//cout << "centripetalForce = " << cf << endl;

		//m_car->AddForce(cf, posInWorldSpace);
	}
	else
	{
		m_centripetalForce = Vector3f::ZERO;
		m_lateralForce = 0.0f;
	}*/
}


void Tire::SetWeightLoad(float weight)
{
	m_weight = weight;
}

void Tire::AddTransmissionTorque(float torque)
{
	m_torque += torque;
}

Vector3f Tire::CalculateForce(float timeDelta)
{
	CalcDerivedData();

	Vector3f forwardAxis = -m_tireToWorld.GetZBasis();
	Vector3f rightAxis = m_tireToWorld.GetXBasis();
	
	// Get the tire's velocity in tire space.
	Vector3f worldPosition = m_tireToWorld.TransformAffine(Vector3f::ZERO);
	Vector3f relativeVelocity = m_car->GetVelocityAtPoint(worldPosition);
	relativeVelocity = m_worldToTire.Rotate(relativeVelocity);
	Vector3f worldVelocity = m_car->GetVelocityAtPoint(worldPosition);
	
	//-------------------------------------------------------------------------
	// Calculate slip angle.
	// "slip angle is capped at PI/2, higher values are considered as travelling in reverse."
	// https://github.com/Siorki/js13kgames/wiki/2.--Car-Physics

	if (relativeVelocity.z > 0.0f)
	{
		relativeVelocity.z = -relativeVelocity.z;
	}
	m_slipAngle = Math::ATan2(relativeVelocity.x, -relativeVelocity.z);
	//m_slipAngle = Math::Clamp(m_slipAngle, -Math::HALF_PI, Math::HALF_PI);
	m_slipAngle = Math::Clamp(m_slipAngle, -Math::PI, Math::PI);
	if (relativeVelocity.x == 0.0f && relativeVelocity.z == 0.0f)
		m_slipAngle = 0.0f;

	//-------------------------------------------------------------------------
	// Calculate slip ratio.

	float tirePatchSpeed = m_angularVelocity * m_radius;
	//float carSpeed = worldVelocity.Length() * Math::Sign(worldVelocity.Dot(forwardAxis));
	float carSpeed = worldVelocity.Dot(forwardAxis);
	m_slipRatio = Math::Sign(tirePatchSpeed);
	if (Math::Abs(carSpeed) > 0.1f)
		m_slipRatio = (tirePatchSpeed - carSpeed) / Math::Abs(carSpeed);

	//-------------------------------------------------------------------------
	// Calculate longitudinal grip.

	m_longitudinalGrip = m_pacejkaLongitudinal.Calcluate(m_slipRatio);
	//if (Math::Abs(worldVelocity.Length()) < 0.6f)
		//m_longitudinalGrip = (m_torque / m_radius) / 30.0f;
	if (Math::Abs(relativeVelocity.Length()) < 0.25f)
		m_longitudinalGrip = Math::Sign(tirePatchSpeed - carSpeed);
	//if (!m_isDrivenWheel)
		//m_longitudinalGrip = 0.0f;

	//-------------------------------------------------------------------------
	// Calculate lateral grip.

	float cornerStiffness = 5.0f;
	float maxGrip = 1.0f;
	m_lateralGrip = -Math::Clamp(m_slipAngle * cornerStiffness, -maxGrip, maxGrip);
	//m_lateralGrip = -m_pacejkaLateral.Calcluate(m_slipAngle);
	if (Math::Abs(relativeVelocity.Length()) < 0.1f)
		m_lateralGrip = 0.0f;

	//-------------------------------------------------------------------------
	// Clamp grip to traction circle.

	Vector2f grip(m_lateralGrip, m_longitudinalGrip);
	//float latScale = 1.0f;
	//float longScale = 1.5f;
	float gripLength = grip.Length();//Math::Sqrt((grip.x * grip.x) / latScale + (grip.y * grip.y) / longScale);//.Length();
	if (gripLength > 1.0f)
	{
		m_lateralGrip = grip.x / gripLength;
		m_longitudinalGrip = grip.y / gripLength;
		//m_lateralGrip = latScale * grip.x / gripLength;
		//m_longitudinalGrip = longScale * grip.y / gripLength;
	}

	//-------------------------------------------------------------------------
	// Calculate forces.

	worldPosition = m_tireToWorld.TransformAffine(Vector3f::ZERO);
	Vector3f velocity = m_car->GetVelocityAtPoint(worldPosition);
	carSpeed = velocity.Length() * Math::Sign(velocity.Dot(forwardAxis));
	
	if (velocity.Length() > 0.6f)
	{
		//m_torque -= (m_longitudinalGrip * m_weight) / (m_weight * m_inertia * m_radius / m_car->m_gravity);
	}
	else if (m_torque == 0.0f && m_isDrivenWheel)
	{
		//m_angularVelocity *= 0.9f;
		//m_car->m_velocity *= 0.9f;
	}
	
	// Lateral force.
	m_lateralForce = m_lateralGrip * m_weight;

	// Longitudinal force.
	m_longitudinalForce = (m_isDrivenWheel ? m_longitudinalGrip * m_weight : 0.0f);

	// Apply torque from traction force.
	//if (m_car->m_velocity.Length() > 0.1f)
		//m_torque -= m_longitudinalForce * m_radius * 0.1f;

    // Integrate torque and angular velocity.
    //m_angularVelocity += (m_torque / m_inertia) * timeDelta;
	m_rotationAngle += m_angularVelocity * timeDelta;
    
	// Clear our torque accumulator.
    //m_torque = 0.0f;
	
	// Sum forces into one vector.
	Vector3f force = Vector3f::ZERO;
	force += forwardAxis * m_longitudinalForce;
	force += rightAxis * m_lateralForce;
	return force;
}

void Tire::ClearAccumulators()
{
	m_torque = 0.0f;
}
