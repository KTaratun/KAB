#include "stdafx.h"
#include "Interpolator.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <DirectXMath.h>
#include <assert.h>

using namespace DirectX;

Interpolator::Interpolator() : animPtr(nullptr)
{
	currentFrame = NULL;
	currentTime = 0;
}


Interpolator::~Interpolator()
{
}

KeyFrame Interpolator::Process(float time) // time should be added
{
	// You set the animation pointer, right?
	assert(animPtr);

	if (currentFrame == NULL)
		currentFrame = animPtr->keyFrames[0];
	
	//
	//currentFrame = currentFrame->GetNext();
	//return *currentFrame;



	// Get new time 
	AddTime(time);
	float finalTime = (*animPtr->keyFrames[animPtr->keyFrames.size() - 1]).GetKeyTime().GetSecondDouble();
	if (currentTime > finalTime)
	{
		currentTime = 0;
		currentFrame = animPtr->keyFrames[0];
	}

	float cFrameTime = (float)currentFrame->GetKeyTime().GetSecondDouble();
	float nFrameTime = (float)currentFrame->GetNext()->GetKeyTime().GetSecondDouble();
	while (currentTime > nFrameTime)
	{
		currentFrame = currentFrame->GetNext();
		cFrameTime = nFrameTime;
		nFrameTime = (float)currentFrame->GetNext()->GetKeyTime().GetSecondDouble();
	}
	float frameTime = nFrameTime - cFrameTime;
	float tweenTime = nFrameTime - currentTime;
	if (frameTime != tweenTime)
		int x = 0;

	float delta = 1 - (tweenTime / frameTime);
	//float timeDelta = frameTime / tweenTime;

	return Interpolate(currentFrame, currentFrame->GetNext(), delta);

	// Make sure currentTime is valid, assuming we want to loop animations
	//float animDuration = animPtr->GetDuration();
	//while(currentTime > animDuration)
	//	currentTime -= animDuration;

	// TODO: Interpolate you keyframe or channel data here
}

KeyFrame Interpolator::Interpolate(KeyFrame* current, KeyFrame* next, float delta)
{
	KeyFrame newKeyFrame;

	//int x = 0;
	//for (size_t bone = 0; bone < 4; bone++)
	//	for (size_t row = 0; row < 4; row++)
	//		for (size_t column = 0; column < 4; column++)
	//			if (current->bones[bone].r[row].m128_f32[column] != next->bones[bone].r[row].m128_f32[column])
	//				x += 1;

	for (size_t i = 0; i < current->bones.size(); i++)
	{
		XMVECTOR scaleCur, scaleNext, rotationCur, rotationNext, positionCur, positionNext;

		XMMatrixDecompose(&scaleCur, &rotationCur, &positionCur, current->bones[i]);
		XMMatrixDecompose(&scaleNext, &rotationNext, &positionNext, next->bones[i]);
		XMVECTOR rotNow = XMQuaternionSlerp(rotationCur, rotationNext, delta);
		XMVECTOR scaleNow = XMVectorLerp(scaleCur, scaleNext, delta);
		XMVECTOR posNow = XMVectorLerp(positionCur, positionNext, delta);
		XMMATRIX MatrixNow = XMMatrixAffineTransformation(scaleNow, XMVectorZero(), rotNow, posNow);
		newKeyFrame.bones.push_back(MatrixNow);
	}

	//float newKeyTime = (next->GetKeyTime().GetSecondDouble() * delta) + (current->GetKeyTime().GetSecondDouble() * (1 - delta));
	//newKeyFrame.SetKeyTime(newKeyTime);
	return newKeyFrame;
}