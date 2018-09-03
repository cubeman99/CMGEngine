#ifndef _CMG_INPUT_INPUT_CONTROL_H_
#define _CMG_INPUT_INPUT_CONTROL_H_

#include <cmgMath/cmgMathLib.h>

class InputControl
{
public:
	InputControl();

	void AddAmount(float amount);
	float GetAmount() const;

private:
	float m_amount;
};

inline InputControl::InputControl() :
	m_amount(0.0f)
{
}

inline void InputControl::AddAmount(float amount)
{
	m_amount += amount;
}

inline float InputControl::GetAmount() const
{
	return Math::Clamp(m_amount, -1.0f, 1.0f);
}

#endif // _CMG_INPUT_INPUT_CONTROL_H_