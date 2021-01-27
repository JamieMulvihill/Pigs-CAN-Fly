#include "State.h"

State::DIFFICULTY State::difficulty = State::EASY;

State::State(gef::Platform & p) :
	platform_(p)
{
	
}
/// function for lerping values
float State::Lerp(float start, float end, float time)
{
	float lerpValue = start + (end - start) * time;

	return lerpValue;
}