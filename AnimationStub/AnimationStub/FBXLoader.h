#pragma once
#include <string>
#include <vector>
#include <fbxsdk.h>

#include "Mesh.h"
#include "TransformNode.h"
#include "Animation.h"

namespace FBXLoader
{
	// Loads an FBX file and returns a filled out mesh, transform hierarchy and animation
	bool Load(const std::string &fileName,
		// [out] Test nmodels provided will only have one mesh, but other assets may have multiple
		// meshes using the same rig to create a model
		std::vector<Mesh > &meshes, 
		// [out] A container of all the joint transforms found. As these will all be in the same 
		// hierarchy, you may only need the root instead of a list of all nodes.
		std::vector<TransformNode> &transformHierarchy,
		// [out] The loaded animation
		Animation &animation, std::vector< unsigned int > &_control_point_indices);
};

