#pragma once
#include "Animation.h"
#include <fbxsdk.h>

class Interpolator
{
public:
	Interpolator();
	~Interpolator();

	inline void AddTime(float timeToAdd) { currentTime += timeToAdd; }
	inline void SetTime(float _currentTime) { currentTime = _currentTime; }
	inline void SetAnimPtr(Animation *aP) { animPtr = aP; }
	inline Animation* GetAnimPtr() { return animPtr; }
	KeyFrame Interpolate(KeyFrame* current, KeyFrame* next, float delta);
	KeyFrame SwitchAnimation(Animation *ani2);

	// Creates the "betweenKeyFrame" data based on the animation and currentTime elements
	KeyFrame Process(float time);
	//bool SWITCH = false;

private:
	float currentTime;

	KeyFrame* currentFrame;

	// The animation this interpolator is interpolating
	Animation *animPtr;//, *animPtr2;

	// The result of the interpolation, if not using channels
	KeyFrame betweenKeyFrame;

	// Or if using channels, we would have one result per joint
	//KeyFrame betweenKeyFrameChannel[NUMBER_OF_JOINTS_IN_RIG]
};

