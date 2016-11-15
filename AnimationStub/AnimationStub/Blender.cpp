#include "stdafx.h"
#include "Blender.h"

void Blender::SetBones(int numBones)
{
	boneOffsetArray.resize(numBones);
	blendTime = 0;
}

XMMATRIX Blender::GetSkinningMatrix(int index)
{
	return boneOffsetArray[index];
}

void Blender::SetAnim(Interpolator* ani)
{
	currAnim = ani;
}

void Blender::SetNextAnim(Interpolator* ani)
{
	nextAnim = ani;
}

void Blender::Update(float delta, std::vector<Vertex> verts, std::vector<TransformNode> hierarchy)
{
	KeyFrame keyFrame = currAnim->Process(delta);

	// SWITCHING ANIMATION
	if (GetAsyncKeyState(VK_TAB) && pressed != true && blendTime <= 0)
	{
		blendTime = .2;
		pressed = true;
	}
	else if (!GetAsyncKeyState(VK_TAB))
		pressed = false;

	if (blendTime > 0)
	{
		blendTime -= delta;
		float aniTransitionTime = .2f;
		float tweenTime = aniTransitionTime - blendTime;
		float lambda = (tweenTime / aniTransitionTime);

		KeyFrame NextAniFrame = nextAnim->Process(delta);
		keyFrame = currAnim->Interpolate(&keyFrame, &NextAniFrame, lambda);

		if (blendTime <= 0)
		{
			Interpolator* temp = currAnim;
			currAnim = nextAnim;
			nextAnim = temp;
		}
	}
	
	// SETTING UP BONE OFFSETS
	for (size_t i = 0; i < hierarchy.size(); i++)
	{
		XMMATRIX bO = hierarchy[i].GetInvBind() * keyFrame.bones[i];
		boneOffsetArray[i] = bO;
	}

	// UPDATING BONE DATA
	//for (UINT i = 0; i < keyframe.bones.size(); i++)
	//	XMStoreFloat4x4(&boneSpheres[i]->worldMatrix.objectMatrix, XMMatrixMultiply(boneScaleMatrix, keyframe.bones[i]));
}