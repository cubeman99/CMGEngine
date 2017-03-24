#include "TorqueCurve.h"


TorqueCurve::TorqueCurve()
{

}

float TorqueCurve::GetTorqueAtSpeed(float engineSpeed) const
{
	if (engineSpeed < m_peakTorque.engineTorque)
		return LerpTorque(TorqueCurveDataPoint(0, 0), m_peakTorque, engineSpeed);
	else if (engineSpeed < m_redLine.engineSpeed)
		return LerpTorque(m_peakTorque, m_redLine, engineSpeed);
	else
		return m_peakTorque.engineTorque;
		//return 0.0f;
}

float TorqueCurve::GetRedLineEngineSpeed() const
{
	return m_redLine.engineSpeed;
}

void TorqueCurve::SetPeakTorque(float peakTorque, float engineSpeed)
{
	m_peakTorque.engineTorque = peakTorque;
	m_peakTorque.engineSpeed = engineSpeed;
}

void TorqueCurve::SetRedLine(float redLineTorque, float redLineEngineSpeed)
{
	m_redLine.engineTorque = redLineTorque;
	m_redLine.engineSpeed = redLineEngineSpeed;
}

void TorqueCurve::SetPeakPower(float peakPower, float engineSpeed)
{
	// power = torque * speed
	float torque = peakPower / engineSpeed;
	SetRedLine(torque, engineSpeed);
}


float TorqueCurve::LerpTorque(
	const TorqueCurveDataPoint& a,
	const TorqueCurveDataPoint& b,
	float engineSpeed)
{
	float t = (engineSpeed - a.engineSpeed) / (b.engineSpeed - a.engineSpeed);
	return (((1.0f - t) * a.engineTorque) + (t * b.engineTorque));
}

