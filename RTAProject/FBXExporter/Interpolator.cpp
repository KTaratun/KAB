#include "stdafx.h"
#include "Interpolator.h"

#include <assert.h>

Interpolator::Interpolator() : animPtr(nullptr)
{
}


Interpolator::~Interpolator()
{
}

void Interpolator::Process()
{
	// You set the animation pointer, right?
	assert(animPtr);

	// Make sure currentTime is valid, assuming we want to loop animations
	float animDuration = animPtr->GetDuration();
	while(currentTime > animDuration)
		currentTime -= animDuration;

	// TODO: Interpolate you keyframe or channel data here
}
