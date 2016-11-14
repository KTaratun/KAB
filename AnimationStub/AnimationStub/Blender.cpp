#include "stdafx.h"
#include "Blender.h"

void Blender::SetBones(int numBones)
{
	boneOffsetArray.resize(numBones);
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
	//if (GetAsyncKeyState(VK_TAB) && pressed != true)
	//{
	//	pressed = true;
	//	//interp.SWITCH = true;
	//	if (interp.GetAnimPtr() == &animations[0])
	//		//interp.SwitchAnimation(&animations[1]);
	//		interp.SetAnimPtr(&animations[1]);
	//	else
	//		//interp.SwitchAnimation(&animations[0]);
	//		interp.SetAnimPtr(&animations[0]);
	//}
	//
	//if (!GetAsyncKeyState(VK_TAB))
	//	pressed = false;
	
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