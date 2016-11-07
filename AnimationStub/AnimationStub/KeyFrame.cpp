#include "stdafx.h"
#include "KeyFrame.h"


KeyFrame::KeyFrame()
{
}


KeyFrame::~KeyFrame()
{
}

int KeyFrame::GetKeyFrameNum()
{
	return keyFrameNum;
}

void KeyFrame::SetKeyFrameNum(int num)
{
	keyFrameNum = num;
}

FbxAMatrix KeyFrame::GetGlobalTransform()
{
	return globalTransform;
}

void KeyFrame::SetGlobalTransform(FbxAMatrix mat)
{
	globalTransform = mat;
}

KeyFrame** KeyFrame::GetNext()
{
	return next;
}

void KeyFrame::SetNext(KeyFrame** _next)
{
	next = _next;
}