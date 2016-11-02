#include "stdafx.h"
#include "AnimStubApp.h"
#include "FBXLoader.h"
#include "Interpolator.h"

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
	FBXLoader::Load("Teddy_Idle.fbx", meshes, transformHierarchy, animation);

	Interpolator test;
	test.Process();
}

void AnimStubApp::Update()
{

}

void AnimStubApp::Render()
{

}
