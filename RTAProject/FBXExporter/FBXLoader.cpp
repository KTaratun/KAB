#include "stdafx.h"
#include "FBXLoader.h"
#include "KeyFrame.h"
#include <set>

namespace FBXLoader
{

	////////////////////////////////////////////////////////////////////////////
	// Forward declaration of internal methods used by FBXLoader::Load method
	//
	// Notes: LoadCurve and GetKeyTimes are provided for you. LoadTexture has code for extracting
	// texture filename but you will need to add functionality for what to do with that file.
	// The rest of the methods will need to be filled in but have high level directions for 
	// you to use.

	////////////////////////////////////////////////////////////////////////////
	// LoadHierarchy:    Load the hierarchy from the FBX file.  The
	//                   hierarchy.GetNodes( ) and fbx_joints arrays are
	//                   parallell.
	// [in]scene:        The FBX scene.
	// [out]heirarchy:   The collection of transformNodes in our hierarchy.
	// [out]fbx_joints:  The FBX representation of the hierarchy.
	// return:           True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadHierarchy(FbxScene* scene, std::vector<TransformNode> &heirarchy,
		std::vector< FbxNode* >& fbx_joints);

	////////////////////////////////////////////////////////////////////////////
	// LoadHierarchy:    Link the hierarchy together.  This function will set
	//                   parent indices and children indices.
	// [out]heirarchy:   The collection of transformNodes in our hierarchy.
	// [in]fbx_joints:   The FBX representation of the hierarchy.
	// return:           True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LinkHierarchy(std::vector<TransformNode> &hierarchy,
		std::vector< FbxNode* >& fbx_joints, FbxNode* root);

	////////////////////////////////////////////////////////////////////////////
	// LoadMesh:                    Load a mesh from an FbxMesh attribute.
	// [in]fbx_mesh:                The FbxMesh attribute to load data from.
	// [out]mesh:                   The mesh data loaded from the FbxMesh
	//                              attribute.
	// [in]fbx_joints:              The FBX representation of the hierarchy.
	// [out]control_point_indices:  The control point indices from which the
	//                              unique vertex originated.  This array and
	//                              the unique vertex array in the mesh would
	//                              be parallell containers.
	// return:                      True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadMesh(FbxMesh* fbx_mesh, Mesh& mesh,
		std::vector< FbxNode* >& fbx_joints,
		std::vector< unsigned int >& control_point_indices);

	//////////////////////////////////////////////////////////////////////////////
	//// GetBindPose:  Get the bind pose transform of a joint.
	//// [in]cluster   The FBX cluster.
	//// return:       The bind pose matrix.
	//////////////////////////////////////////////////////////////////////////////
	FbxAMatrix GetBindPose(FbxNode* mesh_node, FbxCluster* cluster);

	////////////////////////////////////////////////////////////////////////////
	// LoadSkin:                   Load skin data from an FbxMesh attribute.
	// [in]fbx_mesh:               The FbxMesh attribute to load data from.
	// [out]mesh:                  The mesh data loaded from the FbxMesh
	//                             attribute.
	// [out]heirarchy:             The collection of transformNodes in our hierarchy.
	// [out]fbx_joints:            The FBX joints loaded from the scene.
	// [in]control_point_indices:  The control point indices from which the
	//                             unique vertex originated.  This array and
	//                             the unique vertex array in the mesh are
	//                             parallell arrays.
	// return:                     True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadSkin(FbxMesh* fbx_mesh, Mesh& mesh, std::vector<TransformNode> &hierarchy,
		std::vector< FbxNode* >& fbx_joints,
		std::vector< unsigned int >& control_point_indices);


	////////////////////////////////////////////////////////////////////////////
	// LoadTexture:   Load the texture name from a mesh.
	// [in]fbx_mesh:  The FbxMesh attribute to load data from.
	// [out]mesh:     The mesh data loaded from the FbxMesh attribute.
	// return:        True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadTexture(FbxMesh* fbx_mesh, Mesh& mesh);

	////////////////////////////////////////////////////////////////////////////
	// LoadAnimation:   Load an animation.
	// [in]anim_stack:  The animation stack to load the animation from.
	// [in]heirarchy:   The hierarchy.
	// [out]animation:  The loaded animation.
	// [in]fbx_joints:  The fbx_joints loaded from the scene.
	// Return:          True on success, false on failure.
	////////////////////////////////////////////////////////////////////////////
	bool LoadAnimation(FbxAnimStack* anim_stack, std::vector<TransformNode> &hierarchy,
		Animation &animation, std::vector< FbxNode* >& fbx_joints, FbxScene* fbxScene);

	////////////////////////////////////////////////////////////////////////////
	// LoadCurve:       Load the list of unique key times for a single curve.
	// [in]anim_curve:  The animation curve to use.
	// [out]key_times:  The list of unique key times for this animation curve.
	////////////////////////////////////////////////////////////////////////////
	static void LoadCurve(FbxAnimCurve* anim_curve,
		std::set< float >& key_times);

	////////////////////////////////////////////////////////////////////////////
	// KeyReduction:       Perform key reduction on an animation.
	// [in/out]animation:  The animation to perform key reduction on.
	////////////////////////////////////////////////////////////////////////////
	static void KeyReduction(Animation &animation);

	//
	// End Forward declaration of internal methods used by FBXLoader::Load method
	////////////////////////////////////////////////////////////////////////////

	static bool Load(const std::string &fileName,
		// [out] Test nmodels provided will only have one mesh, but other assets may have multiple
		// meshes using the same rig to create a model
		std::vector<Mesh > &meshes,
		// [out] A container of all the joint transforms found. As these will all be in the same 
		// hierarchy, you may only need the root instead of a list of all nodes.
		std::vector<TransformNode> &transformHierarchy,
		// [out] The loaded animation
		Animation &animation)
	{
		// Get an FBX manager
		FbxManager* manager = FbxManager::Create();
		if (manager == 0)
		{
			return false;
		}

		// Create IO settings
		FbxIOSettings* io_settings = FbxIOSettings::Create(manager, IOSROOT);
		if (io_settings == 0)
		{
			return false;
		}

		manager->SetIOSettings(io_settings);

		// Create importer
		FbxImporter* importer = FbxImporter::Create(manager, "");
		if (importer == 0)
		{
			return false;
		}

		// Initialize importer
		if (importer->Initialize(fileName.c_str(), -1, io_settings) == false)
		{
			std::string error;
			error += "FbxImporter::Initialize failed:  ";
			error += importer->GetStatus().GetErrorString();
#ifdef UNICODE
			std::wstring wideError = std::wstring(error.begin(), error.end());
			MessageBox(NULL, wideError.c_str(), L"Error", MB_OK);
#else
			MessageBox(NULL, error.c_str(), "Error", MB_OK);
#endif
			return false;
		}

		// Create a scene
		FbxScene* scene = FbxScene::Create(manager, "myScene");
		if (scene == 0)
		{
			return false;
		}

		// Load the scene with contents from the file
		if (importer->Import(scene) == false)
		{
			return false;
		}

		// No longer need the importer
		importer->Destroy();

		// Make sure everything is triangulated
		FbxGeometryConverter converter(manager);
		if (converter.Triangulate(scene, true) == false)
		{
			return false;
		}

		// If multiple materials are applied to a single mesh, split the mesh into separate meshes
		if (converter.SplitMeshesPerMaterial(scene, true) == false)
		{
			return false;
		}

		// Our list of unique joints
		std::vector< FbxNode* > fbx_joints;

		// Load hierarchy
		if (LoadHierarchy(scene, transformHierarchy, fbx_joints) == false)
		{
			return false;
		}

		// Link hierarchy
		if (LinkHierarchy(transformHierarchy, fbx_joints, scene->GetRootNode()) == false)
		{
			return false;
		}

		// Load Mesh data
		for (int i = 0; i < scene->GetSrcObjectCount< FbxMesh >(); ++i)
		{
			FbxMesh* mesh_attribute = scene->GetSrcObject< FbxMesh >(i);

			Mesh mesh;
			std::vector< unsigned int > control_point_indices;

			if (LoadMesh(mesh_attribute, mesh, fbx_joints, control_point_indices) == false)
			{
				return false;
			}
			if (LoadSkin(mesh_attribute, mesh, transformHierarchy, fbx_joints, 
				control_point_indices) == false)
			{
				return false;
			}
			if (LoadTexture(mesh_attribute, mesh) == false)
			{
				return false;
			}

			meshes.push_back(mesh);
		}

		// Get the number of animation stacks
		int num_anim_stacks = scene->GetSrcObjectCount< FbxAnimStack >();

		FbxAnimStack* anim_stack;
		for (int i = 0; i < num_anim_stacks; ++i)
		{
			// Get the current animation stack
			anim_stack = scene->GetSrcObject< FbxAnimStack >(i);

			animation.SetName(anim_stack->GetName());

			if (LoadAnimation(anim_stack, transformHierarchy, animation, fbx_joints, scene) == false)
			{
				return false;
			}
		}

		// Perform key reduction on the animation
		KeyReduction(animation);

		return true;
	}

	bool LoadHierarchy(FbxScene* scene, std::vector<TransformNode> &transformHierarchy,
		std::vector< FbxNode* >& fbx_joints)
	{
		int numObjs = scene->GetSrcObjectCount();
		for (int i = 0; i < numObjs; i++)
		{
			FbxNode* obj = (FbxNode*) scene->GetSrcObject(i);
			int attrCount = obj->GetNodeAttributeCount();
		
			for (int j = 0; j < attrCount; j++)
			{
				FbxNodeAttribute* nodeAttr = obj->GetNodeAttributeByIndex(j);
				if (nodeAttr->GetAttributeType() == FbxNodeAttribute::eSkeleton || nodeAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					TransformNode *tN = new TransformNode();
					tN->SetName(obj->GetName());
					transformHierarchy.push_back(*tN);
					fbx_joints.push_back(obj);
					break;
				}
			}
		}
		return true;
		return false;
/*
		TODO
		The FBXLoader::LoadHierarchy function will load a vector of all FbxNode objects 
		that have skeleton node attributes or mesh attributes attached. You can use the 
		FbxScene::GetSrcObjectCount and FbxScene::GetSrcObject functions to iterate through 
		all of the FbxNode objects in the scene. When you find an FbxNode with a skeleton or 
		mesh attribute attached, add an entry to both the fbx_joints container and the 
		transformHierarchy container. Those containers are parallel containers and will be used 
		in the LinkHierarchy method to setup the parent and child relationships in the hierarchy.
		Make sure to set the 
		HierarchyNode objects name. The name can be obtained by calling 
		FbxNodeAttribute::GetName. The following API functions are relevant to this function:
			�	FbxScene::GetSrcObjectCount
			�	FbxScene::GetSrcObject
			�	FbxNode::GetNodeAttributeCount
			�	FbxNode::GetNodeAttributeByIndex
			�	FbxNodeAttribute::GetAttributeType
*/
	}

	bool LinkHierarchy(std::vector<TransformNode> &transformHierarchy,
		std::vector< FbxNode* >& fbx_joints, FbxNode* root)
	{
		for (size_t i = 0; i < fbx_joints.size(); i++)
		{
			std::string rootName = root->GetName();
			FbxNode* par = fbx_joints[i]->GetParent();

			if (par->GetName() == rootName)
				transformHierarchy[i].SetParent(NULL);

			for (size_t j = 0; j < fbx_joints.size(); j++)
				if (par->GetName() == transformHierarchy[j].GetName())
				{
					transformHierarchy[i].SetParent(&transformHierarchy[j]);
					transformHierarchy[j].AddChild(&transformHierarchy[i]);
				}
					
			
			//temp.child = transformHierarchy[i]
		
			//Set Transform Node based on the corresponding fbx_joint's GetParent() return
		}
		
		return true;
		return false;

/*	
	TODO
	The FBXLoader::LinkHierarchy function is responsible for setting the parent and 
	children for the TransformNode objects in the hierarchy. You should loop through 
	all of the fbx_joint elements, and set those indices appropriately.
	For each node, you should attempt to find its parent in the fbx_joints array. If the 
	parent does not exist in the array, you can go to the next parent and attempt to find that.
	If you make it past the root of the tree, then your current node is a root node. Make 
	sure to its parent to null to designate this. 
	If you found a parent node, then set the parent of your current node to that 
	node.  At this point, you can also add the current node as a child 
	of the parent node. The following API function is of interest here :
		�	FbxNode::GetParent*/

	}

	int FindJointIndex(const std::string& inJointName, std::vector<FbxNode*> joints)
	{
		for (unsigned int i = 0; i < joints.size(); i++)
			if (joints[i]->GetName() == inJointName)
				return i;
		return -1;
	}

	void ProcessControlPoint(FbxMesh* mesh, std::vector<CtrlPoint*>& cPs)
	{
		unsigned int ctrlPointCount = mesh->GetControlPointsCount();
		for (unsigned int i = 0; i < ctrlPointCount; i++)
		{
			CtrlPoint* currCtrlPoint = new CtrlPoint();
			DirectX::XMFLOAT3 currPosition;
			currPosition.x = static_cast<float>(mesh->GetControlPointAt(i).mData[0]);
			currPosition.y = static_cast<float>(mesh->GetControlPointAt(i).mData[1]);
			currPosition.z = static_cast<float>(mesh->GetControlPointAt(i).mData[2]);
			currCtrlPoint->pos = currPosition;
			cPs.push_back(currCtrlPoint);
		}
	}

	void BlendingInfoPrecaution(CtrlPoint* cP)
	{
		BlendingIndexWeightPair currBlendingIndexWeightPair;
		currBlendingIndexWeightPair.blendingIndex = 0;
		currBlendingIndexWeightPair.blendingWeight = 0;

		for (size_t i = cP->blendingInfo.size(); i < 4; i++)
			cP->blendingInfo.push_back(currBlendingIndexWeightPair);
	}

	bool LoadMesh(FbxMesh* fbx_mesh, Mesh& mesh,
		std::vector< FbxNode* >& fbx_joints,
		std::vector< unsigned int >& control_point_indices)
	{

		std::vector<CtrlPoint*> mControlPoints;
		ProcessControlPoint(fbx_mesh, mControlPoints);
		FbxVector4* CPs = fbx_mesh->GetControlPoints();
		FbxLayerElementArrayTemplate<FbxVector2> *UVs;
		fbx_mesh->GetTextureUV(&UVs);

		FbxGeometryElementNormal* normalElement = fbx_mesh->GetElementNormal();
		FbxGeometryElementBinormal* biNormalElement = fbx_mesh->GetElementBinormal();
		FbxGeometryElementTangent* tangentElement = fbx_mesh->GetElementTangent();
		
		//std::vector<
		Vertex vert;

		int vertexCounter = 0;
		int polyCount = fbx_mesh->GetPolygonCount();

		for (int polyIndex = 0; polyIndex < polyCount; polyIndex++)
		{
			for (int vertIndex = 0;  vertIndex < fbx_mesh->GetPolygonSize(polyIndex);  vertIndex++)
			{

				int ctrlPointIndex = fbx_mesh->GetPolygonVertex(polyIndex, vertIndex);
				vert.xyz[0] = (float)CPs[ctrlPointIndex].mData[0]; // x
				vert.xyz[1] = (float)CPs[ctrlPointIndex].mData[1]; // y
				vert.xyz[2] = (float)CPs[ctrlPointIndex].mData[2]; // z

				int uVIndex = fbx_mesh->GetTextureUVIndex(polyIndex, vertIndex);
				vert.uvw[0] = (float)UVs->GetAt(uVIndex).mData[0]; // u
				vert.uvw[1] = 1- (float)UVs->GetAt(uVIndex).mData[1]; // v
				vert.uvw[2] = 0; // w

				int normIndex = 0;

				if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
					normIndex = vertIndex;
				else if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					normIndex = normalElement->GetIndexArray().GetAt(vertIndex);

				FbxVector4 normals = normalElement->GetDirectArray().GetAt(normIndex);

				BlendingInfoPrecaution(mControlPoints[vertIndex]);

				vert.weights.x = (float)mControlPoints[vertIndex]->blendingInfo[0].blendingWeight;
				vert.weights.y = (float)mControlPoints[vertIndex]->blendingInfo[1].blendingWeight;
				vert.weights.z = (float)mControlPoints[vertIndex]->blendingInfo[2].blendingWeight;
				vert.weights.w = (float)mControlPoints[vertIndex]->blendingInfo[3].blendingWeight;

				vert.bone.x = (int32_t)mControlPoints[vertIndex]->blendingInfo[0].blendingIndex;
				vert.bone.y = (int32_t)mControlPoints[vertIndex]->blendingInfo[1].blendingIndex;
				vert.bone.z = (int32_t)mControlPoints[vertIndex]->blendingInfo[2].blendingIndex;
				vert.bone.w = (int32_t)mControlPoints[vertIndex]->blendingInfo[3].blendingIndex;

				vert.normals[0] = (float)normals.mData[0];
				vert.normals[1] = (float)normals.mData[1];
				vert.normals[2] = (float)normals.mData[2];

				control_point_indices.push_back(ctrlPointIndex);
				bool found = false;

				for (size_t i = 0; i < mesh.verts.size(); i++)
					if (mesh.verts[i].xyz[0] == vert.xyz[0] && mesh.verts[i].xyz[1] == vert.xyz[1] && mesh.verts[i].xyz[2] == vert.xyz[2])
						found = true;
				
				if (found == false)
					mesh.verts.push_back(vert);
			}
		}

		//for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
		//{
		//	delete itr.get;
		//}
		//mControlPoints.clear();
		
		return true;
		// TODO
		// Get control points - fbx_mesh->GetControlPoints()
		// For each polygon in mesh
			// For each vertex in current polygon
				// Get control point index - fbx_mesh->GetPolygonVertex(...)
				// Get Position of vertex
				// Get Texture Coordinates
				// Get Normals
				// Get any other needed mesh data, such as tangents
				// Iterate through unique vertices found so far...
				// if this vertex is unique add to set of unique vertices
				// Push index of where vertex lives in unique vertices container into index 
				// array, assuming you are using index arrays which you generally should be
			// End For each vertex in current polygon
		// End For each polygon in mesh

	}

	bool LoadSkin(FbxMesh* fbx_mesh, Mesh& mesh, std::vector<TransformNode> &hierarchy,
		std::vector< FbxNode* >& fbx_joints,
		std::vector< unsigned int >& control_point_indices)
	{
		std::vector<CtrlPoint*> cps;
		ProcessControlPoint(fbx_mesh, cps);

		for (int deformerIndex = 0; deformerIndex < fbx_mesh->GetDeformerCount(); deformerIndex++)
		{
			FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

			for (int clusterIndex = 0; clusterIndex < currSkin->GetClusterCount(); clusterIndex++)
			{
				FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
				std::string currJointName = currCluster->GetLink()->GetName();
				unsigned int currJointIndex = FindJointIndex(currJointName, fbx_joints);

				FbxAMatrix bindMatrix = GetBindPose(fbx_joints[currJointIndex], currCluster);

				hierarchy[currJointIndex].SetLocal(bindMatrix);

				unsigned int numIndicies = currCluster->GetControlPointIndicesCount();
				for (size_t i = 0; i < numIndicies; i++)
				{
					BlendingIndexWeightPair currBlendingIndexWeightPair;
					currBlendingIndexWeightPair.blendingIndex = currJointIndex;
					currBlendingIndexWeightPair.blendingWeight = currCluster->GetControlPointWeights()[i];
					cps[currCluster->GetControlPointIndices()[i]]->blendingInfo.push_back(currBlendingIndexWeightPair);
				}
			}
		}
		return true;
		return false;
		/*
		TODO
		The FBXLoader::LoadSkin function is the function used to load skinning information. 
		This function will primarily be tasked with extracting joint influence data,
		weight and index, for each vertex. 
		
		You will loop through the number of skin deformers in the FbxMesh. For each skin 
		deformer, you will loop through the skin clusters. Each skin cluster represents one 
		joint's effect on a cluster of control points. The cluster's link represents the joint.
		You will have to find the index of the joint by searching through the fbx_joints 
		vector. Once you know the index, you will have to calculate the world bind pose 
		transform of the joint. You can call FBXLoader::GetBindPose to obtain this matrix and
		store the transform in the TransformNode at that index in 'hierarchy'.

		You will now need to load influence data from the cluster. Each control point index 
		in the cluster will be used to find an influence for the current joint. You should 
		reject influences if their weight is negligible(something less than 0.001f). 
		This will avoid unnecessary processing. The following API methods are or interest for 
		the implementation of this function:
			�	FbxMesh::GetDeformerCount
			�	FbxMesh::GetDeformer
			�	FbxSkin::GetClusterCount
			�	FbxSkin::GetCluster
			�	FbxCluster::GetLink
			�	FbxCluster::GetControlPointIndicesCount
			�	FbxCluster::GetControlPointIndices
			�	FbxCluster::GetControlPointWeights
*/
	}

	FbxAMatrix GetBindPose(FbxNode* mesh_node, FbxCluster* cluster)
	{
		FbxVector4 translation = mesh_node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 rotation = mesh_node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 scaling = mesh_node->GetGeometricScaling(FbxNode::eSourcePivot);
		FbxAMatrix mesh_geometry_transform = FbxAMatrix(translation, rotation, scaling);

		FbxAMatrix bind_pose_transform;
		cluster->GetTransformLinkMatrix(bind_pose_transform);

		FbxAMatrix cluster_transform;
		cluster->GetTransformMatrix(cluster_transform);

		FbxAMatrix result = mesh_geometry_transform * cluster_transform.Inverse() * bind_pose_transform;

		return result;
	}

	bool LoadTexture(FbxMesh* fbx_mesh, Mesh& mesh)
	{
		FbxProperty property;

		if (fbx_mesh->GetNode() == NULL)
			return false;

		int material_count = fbx_mesh->GetNode()->GetSrcObjectCount< FbxSurfaceMaterial >();

		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode()->GetSrcObject< FbxSurfaceMaterial >(material_index);

			if (surface_material == 0)
				continue;

			int texture_index;

			FBXSDK_FOR_EACH_TEXTURE(texture_index)
			{
				property = surface_material->FindProperty(FbxLayerElement::sTextureChannelNames[texture_index]);

				if (property.IsValid() == false)
					continue;

				int texture_count = property.GetSrcObjectCount< FbxTexture >();

				for (int i = 0; i < texture_count; ++i)
				{
					// Ignore layered textures

					FbxTexture* texture = property.GetSrcObject< FbxTexture >(i);
					if (texture == 0)
						continue;

					FbxFileTexture* file_texture = FbxCast< FbxFileTexture >(texture);
					if (file_texture == 0)
						continue;

					std::string texture_name = file_texture->GetFileName();

					// TODO : something with the texture name here....

					std::string::size_type pos = texture_name.find_last_of("/\\");
					if (pos != std::string::npos)
					{
						texture_name = texture_name.substr(pos + 1);
					}
				}
			}
		}

		return true;
	}


	FbxAMatrix GetGeometryTransformation(FbxNode* inNode)
	{
		if (!inNode)
		{
			throw std::exception("Null for mesh geometry");
		}

		const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

		return FbxAMatrix(lT, lR, lS);
	}

	bool LoadAnimation(FbxAnimStack* anim_stack, std::vector<TransformNode> &hierarchy,
		Animation &animation, std::vector< FbxNode* >& fbx_joints, FbxScene* fbxScene)
	{
		for (size_t j = 0; j < fbx_joints.size(); j++)
		{
			FbxString animStackName = anim_stack->GetName();
			animation.SetName(animStackName.Buffer());
			FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			animation.SetDuration((float)end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1);
			KeyFrame** currAnim = hierarchy[j].GetAnimation();
			FbxAMatrix geometryTransform = GetGeometryTransformation(fbx_joints[j]);
			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new KeyFrame();
				(*currAnim)->SetKeyFrameNum(i);
				FbxAMatrix currentTransformOffset = fbx_joints[j]->EvaluateGlobalTransform(currTime) * geometryTransform;
				//(*currAnim)->SetGlobalTransform(currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime));
				//currAnim = &((*currAnim)->mNext);
			}
		}
		
		return false;
		// TODO
		// The following directions assume you are using channels, which allows for each joint
		// to have a varying number of frames in the animation. This is typicially a strong optimization
		// as some joints will not move as much as others in a given animation. If using channels,
		// we will want to query when each key was made for each joint in the animation, then extract
		// the transform for that joint at the found time. 
		
		// The FBX SDK will give you allow you to extract transformations for a joint at anytime, 
		// even if the asset did not have a key made at the given time. If not using channels, a
		// keyframe of all joint transformations can be found key times that we provide at a fixed rate
		// such as 24, 30 0r 60 frames per second. For additional support extracting animation without
		// using channels, refer to this external source tutorial: 
		// http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582
		// Otherwise...

		// The FBXLoader::LoadAnimation function is responsible for loading the actual animation 
		// data. You will loop through each of the fbx_joint elements, obtaining a single channel 
		// of the animation for each. You can call FBXLoader::GetKeyTimes to obtain all the unique 
		// key times that an FbxNode was keyed at. You will need to loop through all of those key 
		// times, excluding the first, and store a key (transform and time) 
		// in the current channel. The collection of keys will produce the channel of animation. The 
		// first key time is ignored because the first key represents the bind pose in the model 
		// you will be importing. After the animation channels are loaded, you may want to do a 
		// second pass to "sanitize" the data. If any channel has 0 keys, you can add a key with 
		// the bind pose transform. The key time should be set to 0.0f. The following are API calls 
		// of interest :
		//	�	FbxTime::SetSecondDouble
		//	�	FbxNode::EvaluateGlobalTransform
	}

	void GetKeyTimes(FbxAnimStack* anim_stack, FbxNode* node, std::set< float >& key_times)
	{
		FbxAnimCurve* anim_curve;

		// Get the number of animation layers in the current animation stack
		int num_anim_layers = anim_stack->GetMemberCount< FbxAnimLayer >();

		FbxAnimLayer* anim_layer;
		for (int j = 0; j < num_anim_layers; ++j)
		{
			anim_layer = anim_stack->GetMember< FbxAnimLayer >(j);

			anim_curve = node->LclTranslation.GetCurve(anim_layer);
			if (anim_curve != 0)
			{
				LoadCurve(anim_curve, key_times);
			}

			anim_curve = node->LclRotation.GetCurve(anim_layer);
			if (anim_curve != 0)
			{
				LoadCurve(anim_curve, key_times);
			}

			anim_curve = node->LclScaling.GetCurve(anim_layer);
			if (anim_curve != 0)
			{
				LoadCurve(anim_curve, key_times);
			}
		}
	}

	void LoadCurve(FbxAnimCurve* anim_curve, std::set< float >& key_times)
	{
		int key_count = anim_curve->KeyGetCount();
		for (int key = 0; key < key_count; ++key)
		{
			float key_time = (float)anim_curve->KeyGetTime(key).GetSecondDouble();
			key_times.insert(key_time);
		}
	}

	void KeyReduction(Animation& animation)
	{
		// TODO ( optional )
		// We could use this method tio attempt to detect if any keyframes/channel keys are not 
		// needed. We can do this by calculating the difference between neighboring keys 
		// (previous & next) and if this difference is less than some minimum threshold remove one
		// of the keys. We would typically remove the "next" key of the neighboring pair, but there 
		// may be special cases.

		// If you are not using channels, then the difference must be less than the threshold for all 
		// joints at that keyframe. This may seem unlikely to happen, but if you extracted keys from 
		// FBX at a fixed interval, as the external web source provided suggests, then you likely will 
		// have more keys then needed.

		// If you are using channels and extracting keys at the keytimes stored in the asset, which 
		// would have been created by an artist, we only need to check neighboring keys in the given 
		// channel, and do this for each channel. That being said, you are less likely to find keys that
		// can safetly be removed as the artist making the asset may have already done that for you.
		// Given time trying this out can still be useful and informative. You might attempt to see how 
		// high you can set that minimum threshold for a particular asset before lost keys cause the 
		// animation to not look correct. FYI, for rotations with quaternions, keys can typically be
		// properly interpolated as long as they are under 180 degrees different.
	}
};