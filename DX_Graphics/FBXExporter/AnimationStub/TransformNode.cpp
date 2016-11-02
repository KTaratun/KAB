#include "stdafx.h"
#include "TransformNode.h"


TransformNode::TransformNode() : parentPtr(nullptr)
{
	name = "Oh crap, I forgot to set the name";
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

//void TransformNode::SetParent(const Transform &_local)
//{
//	local = _local;
//	MakeDirty();
//}