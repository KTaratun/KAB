#pragma once

#include <fbxsdk.h>
class KeyFrame
{
public:
	KeyFrame();
	~KeyFrame();

	FbxLongLong GetKeyFrameNum();
	void SetKeyFrameNum(FbxLongLong num);
	FbxAMatrix GetGlobalTransform();
	void SetGlobalTransform(FbxAMatrix mat);

private:
	// When in the animation does this KeyFrame happen?
	float keyTime;
	FbxLongLong keyFrameNum;
	FbxAMatrix globalTransform;

	// The representation of the animated "skeleton" (All joints) at the keyTime
	// TransformNode joints[NumJointsInRig];

	// Or.....

	// If using "channels", which is better, each joint would have a channel object
	// which collects the keyframes for that specific joint, allowing each joint to have 
	// a differing number of keyframes, but also requireing each joint to be interpolated
	// instead of the whole animation.

	// Refer to the "Common methods of computer animation" presentation for more information
	// on TransformNode, channels and additional animation topics.
};

