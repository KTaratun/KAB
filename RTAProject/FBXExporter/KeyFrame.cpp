#include "stdafx.h"
#include "KeyFrame.h"


KeyFrame::KeyFrame()
{
}


KeyFrame::~KeyFrame()
{
}

FbxLongLong KeyFrame::GetKeyFrameNum()
{
	return keyFrameNum;
}

void KeyFrame::SetKeyFrameNum(FbxLongLong num)
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