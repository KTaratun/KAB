#include "stdafx.h"
#include "AnimStubApp.h"
#include "../FBXExporter/FBXLoader.h"
#include "../FBXExporter/Interpolator.h"

AnimStubApp::AnimStubApp()
{
}


AnimStubApp::~AnimStubApp()
{
}

void AnimStubApp::Initialize()
{
	std::vector<Mesh > meshes;
	std::vector<TransformNode> transformHierarchy;
	Animation animation;
	//FBXLoader::Load("Box_Jump.fbx", meshes, transformHierarchy, animation);

	//Interpolator test;
	//test.Process();
}

void AnimStubApp::Update()
{

}

void AnimStubApp::Render()
{

}
