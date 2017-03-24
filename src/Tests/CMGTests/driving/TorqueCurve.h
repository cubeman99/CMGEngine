#ifndef _TORQUE_CURVE_H_
#define _TORQUE_CURVE_H_

//#define _UNITS__ 3.14159265358f
#define _UNITS_PI_ 3.14159265358f

namespace units
{
	struct UnitBase
	{
	public:
		UnitBase(float amount, float conversion) :
			amount(amount), conversion(conversion)
		{
		}

		inline operator float() const
		{
			return (amount * conversion);
		}

		inline float si() const
		{
			return (amount * conversion);
		}
		
	private:
		float amount;
		float conversion;
	};

#define DECLARE_UNIT(_name, _conversion) \
	struct _name : public UnitBase\
	{\
	public:\
		_name(float amount) :\
			UnitBase(amount, _conversion)\
		{}\
	}
	
	// Angular velocity
	DECLARE_UNIT (rad_per_sec, 1.0f);
	DECLARE_UNIT (rpm, (2.0f * _UNITS_PI_) / 60.0f);

	// Power
	DECLARE_UNIT (horsepower, 0.00134102f);
	DECLARE_UNIT (watts, 1.f);
	DECLARE_UNIT (killowatts, 1000.f);

	// Force
	DECLARE_UNIT (newton_meters, 1.0f);

};


struct TorqueCurveDataPoint
{
	float engineTorque; // Newton-meters
	float engineSpeed; // radians per second

	TorqueCurveDataPoint() :
		engineTorque(0),
		engineSpeed(0)
	{
	}

	TorqueCurveDataPoint(float engineTorque, float engineSpeed) :
		engineTorque(engineTorque),
		engineSpeed(engineSpeed)
	{
	}
};


class TorqueCurve
{
public:
	TorqueCurve();

	float GetTorqueAtSpeed(float engineSpeed) const;
	float GetRedLineEngineSpeed() const;

	void SetPeakTorque(float peakTorque, float engineSpeed);
	void SetRedLine(float redLineTorque, float redLineEngineSpeed);
	void SetPeakPower(float peakPower, float engineSpeed);

private:
	static float LerpTorque(const TorqueCurveDataPoint& a, const TorqueCurveDataPoint& b, float engineSpeed);

	TorqueCurveDataPoint m_peakTorque; // The point at which peak torque is reached.
	TorqueCurveDataPoint m_redLine; // The point when the engine red-lines.
};


#endif // _TORQUE_CURVE_H_