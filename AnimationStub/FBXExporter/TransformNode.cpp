#include "stdafx.h"
#include "TransformNode.h"


TransformNode::TransformNode() : parentPtr(nullptr)
{
	name = "Oh crap, I forgot to set the name";
	//animation = new KeyFrame();
}


TransformNode::~TransformNode()
{
}

const Transform& TransformNode::GetWorld()
{
	// TODO
	// add code here... 
	return world;
}

void TransformNode::SetLocal(const Transform &_local)
{
	local = _local;
	MakeDirty();
}

void TransformNode::SetParent(TransformNode* parent)
{
	parentPtr = parent;
}

void TransformNode::AddChild(TransformNode* child)
{
	children.push_back(child);
}