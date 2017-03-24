#include "Car.h"

using namespace std;

// Notes:
// Manual shift time = 1 second


//-----------------------------------------------------------------------------
// Constructors and destructor
//-----------------------------------------------------------------------------

Car::Car() :
	m_orientation(Quaternion::IDENTITY),
	m_position(Vector3f::ZERO),
	m_velocity(Vector3f::ZERO),
	m_angularVelocity(Vector3f::ZERO),
	m_engineSpeed(0.0f),
	m_gearIndex(0),
	m_steeringAngle(0.0f),
	m_throttleAmount(0.0f),
	m_handBreakAmount(0.0f)
{
	// Global constants.
	m_gravity = 9.81f;
	m_densityOfAir = 1.225f; // kg/m^3

	// Dimensions (Using BMW E34 sedan).
	m_mass					= 1435.0f; // kg
	m_wheelBase				= 2.761f;
	m_chassisSize.x			= 1.751f;
	m_chassisSize.z			= 4.720f;
	m_chassisSize.y			= 1.412f;
	m_wheelTrack			= 1.620f;
	m_centerOfMassHeight	= m_chassisSize.y * 0.33f;
	m_centerOfMassOffset	= 0.0f;
	m_dragCoefficient		= 0.39f;
	m_frontalArea			= m_chassisSize.z * m_chassisSize.y;
	//m_finalDriveRatio	= 4.10f; // for e34

	// Dimensions (Using Toyota Corolla AE86).
	m_mass					= 1000.0f; // kg
	m_wheelBase				= 2.400f;
	m_chassisSize.x			= 1.630f;
	m_chassisSize.z			= 4.200f;
	m_chassisSize.y			= 1.340f;
	m_wheelTrack			= 1.400f;
	m_centerOfMassHeight	= m_chassisSize.y * 0.33f;
	m_centerOfMassOffset	= 0.0f;
	m_dragCoefficient		= 0.39f;
	m_frontalArea			= m_chassisSize.z * m_chassisSize.y;

	// Derive inertia from chassis as if it was a uniform-density box.
	m_inertiaTensor.InitScale(
		(m_mass / 12.0f) * ((m_chassisSize.z * m_chassisSize.z) + (m_chassisSize.x * m_chassisSize.x)),
		(m_mass / 12.0f) * ((m_chassisSize.y * m_chassisSize.y) + (m_chassisSize.z * m_chassisSize.z)),
		(m_mass / 12.0f) * ((m_chassisSize.x * m_chassisSize.x) + (m_chassisSize.y * m_chassisSize.y)));
	m_inverseInertiaTensor = m_inertiaTensor.GetInverse();

	// Engine.
	m_engineIdleSpeed = units::rpm(800);
	m_engineRedLineSpeed = units::rpm(6800);
	m_torqueCurve.SetPeakTorque(units::newton_meters(392), units::rpm(4400));
	m_torqueCurve.SetPeakPower(units::killowatts(206), units::rpm(6800));

	// Transmission.
	m_numGears = 5;
	m_reverseGearRatio = 4.00f;
	m_gears[0].m_ratio = 3.17f;
	m_gears[1].m_ratio = 1.88f;
	m_gears[2].m_ratio = 1.30f;
	m_gears[3].m_ratio = 1.00f;
	m_gears[4].m_ratio = 0.90f;
	m_finalDriveRatio = 4.444f;
	m_transmissionEfficiency = 0.70f;
	// Taken from inertia calculators at http://hpwizard.com/rotational-inertia.html
	m_driveTrainInertia = 0.36728f + 0.00029475f + 0.079453f + 0.018290f + 0.0065845f + 0.063211f;
	
	// Tires.
	m_brakingTorque = 4000.0f;
	m_wheelRadius = 0.3f;
	m_tireFrictionCoefficient = 1.0f;
	m_rollingResistanceCoefficient = 0.02f;
	m_numWheels = 4;
	for (unsigned int i = 0; i < 4; i++)
	{
		m_tires[i].m_car = this;
		m_tires[i].m_radius = m_wheelRadius;
		m_tires[i].m_isDrivenWheel = (i >= 2);
	}
	m_tires[Tire::k_front_left].m_offset.Set(-m_wheelTrack * 0.5f, 0.0f, -m_wheelBase * 0.5f);
	m_tires[Tire::k_front_right].m_offset.Set(m_wheelTrack * 0.5f, 0.0f, -m_wheelBase * 0.5f);
	m_tires[Tire::k_rear_left].m_offset.Set(-m_wheelTrack * 0.5f, 0.0f, m_wheelBase * 0.5f);
	m_tires[Tire::k_rear_right].m_offset.Set(m_wheelTrack * 0.5f, 0.0f, m_wheelBase * 0.5f);
}

Car::~Car()
{
}

void Car::Reset()
{
	m_position.SetZero();
	m_orientation.SetIdentity();
	m_velocity.SetZero();
	m_angularVelocity.SetZero();
	m_gearIndex = 0;
	m_steeringAngle = 0.0f;
	m_engineSpeed = 0.0f;
	m_throttleAmount = 0.0f;
	m_handBreakAmount = 0.0f;

	m_engineTorque = 0.0f;
	m_wheelTorque = 0.0f;
	m_chassisTilt.SetZero();
	m_weightTransferTorque.SetZero();

	// Reset tires.
	float dafaultTireLoad = (m_mass * m_gravity) / m_numWheels;
	for (unsigned int i = 0; i < m_numWheels; i++)
	{
		m_tires[i].Reset();
		m_tires[i].SetWeightLoad(dafaultTireLoad);
	}

	ClearAccumulators();
}


//-----------------------------------------------------------------------------
// Car dynamics
//-----------------------------------------------------------------------------

// Get the velocity of a point in world space that's connected to the car.
Vector3f Car::GetVelocityAtPoint(const Vector3f& point) const
{
	return (m_velocity + m_angularVelocity.Cross(point - m_position));
}

void Car::CalcDerivedData()
{
	// Transformation matrices.
	m_carToWorld = Matrix4f::CreateTranslation(m_position) * 
		Matrix4f::CreateRotation(m_orientation);
	m_worldToCar = m_carToWorld.GetAffineInverse();
	
	m_engineTorque = m_torqueCurve.GetTorqueAtSpeed(
		Math::Max(m_engineSpeed, m_engineIdleSpeed));
	
	for (unsigned int i = 0; i < 4; i++)
		m_tires[i].CalcDerivedData();
}

void Car::UpdatePhysics(float timeDelta)
{
	CalcDerivedData();
	
	m_netTorque.SetZero();
	m_netForce.SetZero();

	Vector3f forwardVector = m_orientation.GetForward();
	Vector3f rightVector = m_orientation.GetRight();
	Vector3f surfaceVelocity = m_velocity;
	surfaceVelocity.y = 0.0f;
	float speed = surfaceVelocity.Length();
	float G = GetGear().m_ratio * m_finalDriveRatio;

	//-------------------------------------------------------------------------
	// Vertical forces
	
	float liftForce = 0.0f; // TODO: lift force (for spoilers, etc.)
	float weight = m_mass * m_gravity;
	float verticalForce = weight - liftForce;

	if (m_position.y <= 0.0f)
	{
		m_position.y = 0.0f;
		if (m_velocity.y < 0.0f)
			m_velocity.y = 0.0f;
	}
	else
		AddForce(Vector3f::DOWN * m_gravity * m_mass);

	//-------------------------------------------------------------------------
	// Longitudinal forces

	// Rolling resistance. TODO: isn't this per tire?
	m_rollingResistanceForce = -m_rollingResistanceCoefficient *
		verticalForce * Vector3f::Normalize(surfaceVelocity);
	AddForce(m_rollingResistanceForce);

	// Drag force.
	m_dragForce = -(0.5f * m_dragCoefficient *
		m_frontalArea * speed * m_densityOfAir) * surfaceVelocity;
	AddForce(m_dragForce);
	
	//-------------------------------------------------------------------------
	// Steering

	// Update steering angle of tires.
	float turnRadius = 0.0f;
	if (Math::Abs(m_steeringAngle) > 0.01f)
	{
		// Determine the radius of the turn relative to the point between
		// the two rear wheels. This radius will be negative for a negative
		// steering angle.
		float turnRadiusRearCenter = m_wheelBase / Math::Tan(m_steeringAngle);

		// Determine the adjusted steering angles for the two front tires
		// for proper Ackerman steering.
		float turnRadiusRearLeft = turnRadiusRearCenter - (m_wheelTrack * 0.5f);
		float turnRadiusRearRight = turnRadiusRearCenter + (m_wheelTrack * 0.5f);
		m_tires[Tire::k_front_left].m_steeringAngle = Math::ATan(m_wheelBase / turnRadiusRearLeft);
		m_tires[Tire::k_front_right].m_steeringAngle = Math::ATan(m_wheelBase / turnRadiusRearRight);
	}
	
	//-------------------------------------------------------------------------
	// Weight transfer

	float dafaultTireLoad = verticalForce / (float) m_numWheels;
	float addedBackWeight = m_weightTransferTorque.x /
		(m_wheelBase * 0.5f * m_numWheels);
	float addedLeftWeight = m_weightTransferTorque.z /
		(m_wheelTrack * 0.5f * m_numWheels);
	m_tires[Tire::k_front_left].SetWeightLoad(
		dafaultTireLoad - addedBackWeight + addedLeftWeight);
	m_tires[Tire::k_front_right].SetWeightLoad(
		dafaultTireLoad - addedBackWeight - addedLeftWeight);
	m_tires[Tire::k_rear_left].SetWeightLoad(
		dafaultTireLoad + addedBackWeight + addedLeftWeight);
	m_tires[Tire::k_rear_right].SetWeightLoad
		(dafaultTireLoad + addedBackWeight - addedLeftWeight);
	
	//-------------------------------------------------------------------------
	// Calculate torque applied to wheels
	
	// Maximum torque available from the engine.
	m_engineTorque = m_torqueCurve.GetTorqueAtSpeed(
		Math::Max(m_engineSpeed, m_engineIdleSpeed));
	float maxWheelTorque = m_engineTorque * m_transmissionEfficiency * G;
	float maxReversingTorque = -m_engineTorque * m_transmissionEfficiency *
			m_reverseGearRatio * m_finalDriveRatio;

	// Throttle torque.
	float wheelThrottleTorque = maxWheelTorque * m_throttleAmount;
	wheelThrottleTorque += maxReversingTorque * m_reverseAmount;

	// Breaking torque.
	//  * Reversing will count as breaking when moving forwards.
	//  * Throttle will count as breaking when moving backwards.
	float breakAmount = m_handBreakAmount;
	if (m_engineSpeed > 5.0f)
		breakAmount = Math::Max(m_handBreakAmount, m_reverseAmount);
	else if (m_engineSpeed < -5.0f)
		breakAmount = Math::Max(m_handBreakAmount, m_throttleAmount);
	float appliedBrakingTorque = -(m_brakingTorque * breakAmount) * 
		Math::Sign(m_engineSpeed);
	
	// Applying the breaks will cancel out any throttle/reversing torques.
	if (breakAmount > 0.0f)
		wheelThrottleTorque *= 0.2f + (0.8f * (1.0f - breakAmount));
	
	m_wheelTorque = wheelThrottleTorque + appliedBrakingTorque;
	
	//-------------------------------------------------------------------------
	// Update tires
	
	unsigned int numDriveTires = 2;

	// Accumulate some forces.
	float totalLateralForce = 0.0f;
	float totalLongitudinalForce = 0.0f;
	m_tractionForce = Vector3f::ZERO;
	m_weightTransferTorque = Vector3f::ZERO;
	
	// Handle slow speeds.
	bool verySlow = (speed < 1.0f && m_angularVelocity.y < 0.3f);
	if (verySlow)
		m_angularVelocity.y *= 0.9f;
	float slowFactor = 1.0f - ((speed - 0.1f) / 8.0f);
	slowFactor = Math::Clamp(slowFactor, 0.0f, 1.0f);
	float slowFactorAngularToLinearRatio = 6.0f;
	if (Math::Abs(m_wheelTorque) < 0.1f)
		slowFactor = 0.0f;

	// Update each wheel individually.
	for (unsigned int i = 0; i < m_numWheels; i++)
	{
		Tire& tire = m_tires[i];
		
		// Match the tire speed with the engine speed.
		tire.m_angularVelocity = m_engineSpeed / G;

		// Apply throttle and/or braking torque to the wheel.
		if (tire.m_isDrivenWheel)
		{
			tire.AddTransmissionTorque(wheelThrottleTorque / numDriveTires);
			tire.AddTransmissionTorque(appliedBrakingTorque / numDriveTires);
			//tire.AddTransmissionTorque(appliedBrakingTorque / m_numWheels);
		}
		else
		{
			//tire.AddTransmissionTorque(appliedBrakingTorque / m_numWheels);
		}

		// Calculate the wheel's traction force.
		Vector3f wheelResponseForce = tire.CalculateForce(timeDelta);

		if (i < 2)
			totalLateralForce += tire.m_lateralForce;
		else
			totalLateralForce -= tire.m_lateralForce;
		
		if (slowFactor < 0.001f && Math::Abs(speed) < 0.5f)
		{
			m_engineSpeed *= 0.8f;
			m_wheelTorque = 0.0f;
			tire.m_longitudinalForce = 0.0f;
			wheelResponseForce -= wheelResponseForce.Dot(GetForwardAxis()) * GetForwardAxis();
		}
		
		// Accumulate weight transfer torque from the traction force.
		m_weightTransferTorque.x += tire.m_longitudinalForce * m_centerOfMassHeight;
		m_weightTransferTorque.z += tire.m_lateralForce * m_centerOfMassHeight;

		// Adjust forces for slow speeds.
		tire.m_longitudinalForce *= 1.0f + slowFactor * 1.0f;

		// Make sure the force is in the right direction.
		if (!verySlow || ((tire.m_torque >= 0.0f || tire.m_longitudinalForce <= 0.0f) &&
			(tire.m_torque <= 0.0f || tire.m_longitudinalForce >= 0.0f)))
		{
			totalLongitudinalForce += tire.m_longitudinalForce;
			m_tractionForce += wheelResponseForce;
			Vector3f tirePos = tire.m_tireToWorld.TransformAffine(tire.m_offset);
			m_forceAccumulator += wheelResponseForce;
			//AddForce(wheelResponseForce, tirePos);
		}

		tire.ClearAccumulators();
	}
	
	// For some reason, the tires are considered not angled for
	// torque calculations about the up axis.
	m_torqueAccumulator.y -= totalLateralForce * (m_wheelBase * 0.5f);

	//-------------------------------------------------------------------------
	// Torque feedback from tire traction to engine

	// Update engine speed with torque from tires.
	float torqueToEngine = (m_wheelTorque / G);
	torqueToEngine -= (1.0f - slowFactor) *
		(totalLongitudinalForce * m_tires[0].m_radius) / G;
	if (slowFactor > 0.0f)
		torqueToEngine *= slowFactorAngularToLinearRatio * slowFactor;

	// Integrate torque applied back to the engine's drive shaft.
	m_engineSpeed += (torqueToEngine / m_driveTrainInertia) * timeDelta;

	// Allow an engine speed below 0 for driving in reverse.
	//if (m_engineSpeed < m_engineIdleSpeed)
		//m_engineSpeed = m_engineIdleSpeed;
	//if (m_engineSpeed < 0.0f)
		//m_engineSpeed = 0.0f;
	if (m_engineSpeed > m_engineRedLineSpeed + units::rpm(500))
		m_engineSpeed = m_engineRedLineSpeed - units::rpm(500);

	//-------------------------------------------------------------------------
	// Chassis tilt

	// Fake chassis tilt based on weight transfer torque.
	//if (m_velocity.Length() > 3.0f)
	{
		float dampening = 1.0f + (6.0f * m_chassisTilt.Length());
		m_chassisTilt += (m_inverseInertiaTensor * m_weightTransferTorque *
			0.6f * timeDelta) / dampening;
	}
	m_chassisTilt *= Math::Pow(0.5f, timeDelta / 0.2f);
	
	// Integrate force/velocity.
	Integrate(timeDelta);
	ClearAccumulators();

	// Recalculate derived data.
	CalcDerivedData();
	for (unsigned int i = 0; i < m_numWheels; i++)
		m_tires[i].CalcDerivedData();
}

//-----------------------------------------------------------------------------
// Transmission
//-----------------------------------------------------------------------------

void Car::ShiftUp()
{
	if (m_gearIndex >= m_numGears - 1)
		return;

	Gear& prevGear = m_gears[m_gearIndex];
	Gear& nextGear = m_gears[m_gearIndex + 1];
	
	// Adjust the engine speed for the new gear ratio.
	m_engineSpeed *= (nextGear.m_ratio / prevGear.m_ratio);

	m_gearIndex++;
}

void Car::ShiftDown()
{
	if (m_gearIndex == 0)
		return;

	Gear& prevGear = m_gears[m_gearIndex];
	Gear& nextGear = m_gears[m_gearIndex - 1];
	
	// Adjust the engine speed for the new gear ratio.
	m_engineSpeed *= (nextGear.m_ratio / prevGear.m_ratio);

	m_gearIndex--;
}

Gear& Car::GetGear()
{
	return m_gears[m_gearIndex];
}


//-----------------------------------------------------------------------------
// Rigid body dynamics
//-----------------------------------------------------------------------------

void Car::AddForce(const Vector3f& force)
{
	m_forceAccumulator += force;
}

void Car::AddForce(const Vector3f& force, const Vector3f position)
{
	// Add force.
	m_forceAccumulator += force;

	// Add torque.
	Vector3f offsetFromCenter = position - m_position;
	m_torqueAccumulator += offsetFromCenter.Cross(force);
}

void Car::Integrate(float timeDelta)
{
	// Integrate force.
	m_velocity += (m_forceAccumulator / m_mass) * timeDelta;

	// TODO: Integrate torque.
	m_angularVelocity += (m_inverseInertiaTensor * m_torqueAccumulator) * timeDelta;

	// Integrate linear velocity.
	m_position += m_velocity * timeDelta;
	
	// Integrate angular velocity
	Quaternion rotation(Vector4f(m_angularVelocity, 0.0f));
	m_orientation += (rotation * m_orientation) * (timeDelta * 0.5f);
	m_orientation.Normalize();
}

void Car::ClearAccumulators()
{
	m_netForce = m_forceAccumulator;
	m_netTorque = m_torqueAccumulator;

	m_forceAccumulator.SetZero();
	m_torqueAccumulator.SetZero();
}





