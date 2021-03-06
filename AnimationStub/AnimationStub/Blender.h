#pragma once
#include "Interpolator.h"
#include "TransformNode.h"
#include "Mesh.h"

class Blender
{
	std::vector<std::vector<Animation>> AnimationContainer;
	//blend
	std::vector<XMMATRIX> boneOffsetArray;
	Interpolator* currAnim;
	Interpolator* nextAnim;
public:

	bool pressed = false;
	float blendTime = 0;

	void SetBones(int numBones);
	std::vector<XMMATRIX> GetSkinningMatrix(); //Bone Offset
	void SetAnim(Interpolator* ani);
	void SetNextAnim(Interpolator* ani);
	KeyFrame Update(float delta, std::vector<Vertex> verts, std::vector<TransformNode> hierarchy);
	void Blending(float delta);
};