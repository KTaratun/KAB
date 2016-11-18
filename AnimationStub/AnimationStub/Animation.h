#pragma once
#include <string>
#include <vector>
#include "KeyFrame.h"
#include <fbxsdk.h>

//struct Channel
//{
//	// This should not be a constant in your version
//	static const int NUMBER_OF_KEYS_FOR_THIS_JOINT = 42;
//
//	KeyFrame keyFrames[NUMBER_OF_KEYS_FOR_THIS_JOINT];
//};

class Animation
{
public:
	Animation();
	~Animation();

	inline float GetDuration() { return duration; }
	inline void SetDuration(float _duration) { duration = _duration; }

	inline const FbxString &GetName() const { return animName; }
	inline void SetName(const FbxString &name) { animName = name; }

	//KeyFrame* keyFrames[42];
	std::vector<KeyFrame*> keyFrames;
	std::vector<KeyFrame> keyFrames2;
private:
	FbxString animName;

	// The total length of the animation
	float duration;

	//static const int NUMBER_OF_KEYS_FOR_THIS_JOINT = 42;

	// The representation of the animation at a certain point in time
	//KeyFrame keyFrames[NUMBER_OF_KEYS_IN_ANIMATION];

	// Or if using channels, remove the array of keyFrames above and instead do something like:


	//Channel channels[NUMBER_OF_JOINTS_IN_RIG];

};

