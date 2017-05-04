#include "FBXImporter.h"
#include <assert.h>
#include "MeshHeader.h"
#include "HelperStructs.h"
#include <fstream>

//Utility functions
DirectX::XMMATRIX FbxMatrixToXMFLOAT4X4A(FbxAMatrix& matrix)
{
	DirectX::XMFLOAT4X4 outMatrix;
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			outMatrix.m[row][col] = matrix.Get(row, col);
		}
	}
	return DirectX::XMLoadFloat4x4(&outMatrix);
}

///////////////////

FBXImporter::FBXImporter()
{
	manager = FbxManager::Create();
	ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");
}


FBXImporter::~FBXImporter()
{
}

void FBXImporter::ProcessControlPoints(FbxNode* inNode, std::unordered_map<unsigned int, CtrlPoint*> &mControlPoints)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CtrlPoint* currCtrlPoint = new CtrlPoint();
		DirectX::XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint->mPosition = currPosition;
		mControlPoints[i] = currCtrlPoint;
	}
}

void DumpRecursive(FbxNode* current_node, std::vector<Joint> &joints, int current, int parent) {
	if (current_node->GetNodeAttribute() && current_node->GetNodeAttribute()->GetAttributeType() && current_node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
		//Joint found
		Joint j;
		j.name = current_node->GetName();
		j.parent_id = parent;
		if (j.name == "root_jnt") j.parent_id = -1;
		joints.push_back(j);
	}

	for (int b = 0; b < current_node->GetChildCount(); b++) {
		FbxNode *child_node = current_node->GetChild(b);

		DumpRecursive(child_node, joints, joints.size(), current);
	}
}

void FBXImporter::ImportAnimatedMesh(const char * filename, sSkinnedMesh* mesh, vector<sMaterial*>& outMaterials)
{

	importer->Initialize(filename, -1, manager->GetIOSettings());
	scene = FbxScene::Create(manager, "Scene");

	importer->Import(scene);
	
	//done with importer
	importer->Destroy();

	FbxNode* pFbxRootNode = scene->GetRootNode();

	//Read bone hierarchy
	std::vector<Joint> joints;
	DumpRecursive(pFbxRootNode, joints, 0, -1);

	if (pFbxRootNode)
	{
		int nodes = pFbxRootNode->GetChildCount();
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pCurrentNode = pFbxRootNode->GetChild(i);
			if (pCurrentNode->GetNodeAttribute() == NULL) continue;

			FbxNodeAttribute::EType AttributeType = pCurrentNode->GetNodeAttribute()->GetAttributeType();
			if (AttributeType != FbxNodeAttribute::eMesh) continue;

			std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
			ProcessControlPoints(pCurrentNode, mControlPoints);

			FbxMesh* currentMesh = (FbxMesh*)pCurrentNode->GetNodeAttribute();
			
		

			if (!currentMesh) continue; //Just in case

			FbxVector4* pVertices = currentMesh->GetControlPoints();


			//Export UVs
			FbxStringList uvs;
			currentMesh->GetUVSetNames(uvs);
			mesh->header.numberOfUVSets = 0;
			for (int i = 0; i < uvs.GetCount(); i++) {
				mesh->header.numberOfUVSets++;

				UVSet uvset;
				uvset.name = uvs.GetStringAt(0);
				uvset.name_length = uvset.name.length();
				uvset.id = i;

				mesh->uvsets.push_back(uvset);
			}

			FbxDeformer *deformer = nullptr;

			int deformer_count = currentMesh->GetDeformerCount();
			for (int d = 0; d < deformer_count; d++) {
				deformer = currentMesh->GetDeformer(d);
				FbxDeformer::EDeformerType deformer_type = deformer->GetDeformerType();

				if (deformer_type == FbxDeformer::eSkin) break;
			}


			FbxSkin *skin = deformer && deformer->Is<FbxSkin>() ? (FbxSkin*)deformer : 0;
			std::string AnimationName;

			if (skin) {
				int n_of_clusters = skin->GetClusterCount();

				for (int c = 0; c < n_of_clusters; c++) {
					
					FbxCluster *cluster = skin->GetCluster(c);

					//Get current joint index
					std::string currJointName = cluster->GetLink()->GetName();
					unsigned int currJointIndex = FindJointIndexUsingName(currJointName, &joints);

					FbxCluster::ELinkMode cluster_mode = cluster->GetLinkMode();

					FbxAMatrix model_to_pose_mat;
					model_to_pose_mat = cluster->GetTransformLinkMatrix(model_to_pose_mat);
					FbxAMatrix model_to_pose_mat_inv = model_to_pose_mat.Inverse();


					FbxAMatrix bone_init_pose;
					bone_init_pose = cluster->GetTransformMatrix(bone_init_pose);

					FbxAMatrix bind_pose_inv = model_to_pose_mat_inv * bone_init_pose;
					FbxAMatrix bild_pose = bind_pose_inv.Inverse();
					joints[currJointIndex].inverseBindPose = FbxMatrixToXMFLOAT4X4A(bind_pose_inv);

					int n_of_indices = cluster->GetControlPointIndicesCount();
					int *indices = cluster->GetControlPointIndices();
					double *weights = cluster->GetControlPointWeights();

					for (int ind = 0; ind < n_of_indices; ind++) {
						float weight = (float)weights[ind];

						CtrlPoint *ctrl = new CtrlPoint();
						BlendingIndexWeightPair currBlendingIndexWeightPair;
						currBlendingIndexWeightPair.mBlendingIndex = currJointIndex;
						currBlendingIndexWeightPair.mBlendingWeight = cluster->GetControlPointWeights()[ind];
						mControlPoints[cluster->GetControlPointIndices()[ind]]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
					}

					// Get animation information
					// Now only supports one take


					FbxAnimStack* currAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
					FbxString animStackName = currAnimStack->GetName();
					AnimationName = animStackName.Buffer();
					FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
					FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
					FbxTime end = takeInfo->mLocalTimeSpan.GetStop();


					mesh->animation.m_framesPerSecond = 24;
					mesh->animation.m_frameCount = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
					mesh->animation.m_aSamples.resize(mesh->animation.m_frameCount);
					mesh->animation.m_aSamples[currJointIndex].m_aJointPose.resize(joints.size());
					FbxLongLong poop = end.GetFrameCount(FbxTime::eFrames24);
					for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
					{
						FbxTime currTime;
						currTime.SetFrame(i, FbxTime::eFrames24);

						if (mesh->animation.m_aSamples[i-1].m_aJointPose.size() == 0)
							mesh->animation.m_aSamples[i-1].m_aJointPose.resize(joints.size());
						JointPose& currentPose = (mesh->animation.m_aSamples[i-1].m_aJointPose[currJointIndex]);

						FbxAMatrix currentTransformOffset = pCurrentNode->EvaluateGlobalTransform(currTime);
						FbxAMatrix mGlobalTransform = currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateLocalTransform(currTime);

						//currentPose.m_rot = mGlobalTransform.GetQ()[0];
						//currentPose.m_rot.y = mGlobalTransform.GetQ()[1];
						//currentPose.m_rot.z = mGlobalTransform.GetQ()[2];
						//currentPose.m_rot.w = mGlobalTransform.GetQ()[3];
						//
						//currentPose.m_trans.x = mGlobalTransform.GetT()[0];
						//currentPose.m_trans.y = mGlobalTransform.GetT()[1];
						//currentPose.m_trans.z = mGlobalTransform.GetT()[2];
						
						//Get rotation
						DirectX::XMFLOAT4 rot(
							mGlobalTransform.GetQ()[0],
							mGlobalTransform.GetQ()[1],
							mGlobalTransform.GetQ()[2],
							mGlobalTransform.GetQ()[3]);

						//Get rotation
						DirectX::XMFLOAT3 trans(
							mGlobalTransform.GetT()[0],
							mGlobalTransform.GetT()[1],
							mGlobalTransform.GetT()[2]);

						//Load into currentPose
						currentPose.m_rot = DirectX::XMLoadFloat4(&rot);
						currentPose.m_trans = DirectX::XMLoadFloat3(&trans);

						//Scale always 1.0f
						currentPose.m_scale = 1.0f;
					}

				}
			}

			mesh->header.frameCount = mesh->animation.m_frameCount;
			mesh->header.framesPerSecond = mesh->animation.m_framesPerSecond;
			mesh->header.isLooping = mesh->animation.m_isLooping;

			//mesh->animation = *animation;

			for (int j = 0; j < currentMesh->GetPolygonCount(); j++)
			{
				int vertCount = currentMesh->GetPolygonSize(j);
				assert(vertCount == 3);

				for (int k = 0; k < vertCount; k++)
				{
					int iControlPointIndex = currentMesh->GetPolygonVertex(j, k);
					
					//Export vertices
					SkinnedVertex vertex;
					vertex.posX = (float)pVertices[iControlPointIndex].mData[0];
					vertex.posY = (float)pVertices[iControlPointIndex].mData[1];
					vertex.posZ = (float)pVertices[iControlPointIndex].mData[2];
					
					//Export normals
					FbxVector4 normal;
					currentMesh->GetPolygonVertexNormal(j, k, normal);
					vertex.norX = normal[0];
					vertex.norY = normal[1];
					vertex.norZ = normal[2];


					//Export UVs
					FbxStringList uvs;
					currentMesh->GetUVSetNames(uvs);

					for (int u = 0; u < uvs.GetCount(); u++) {

						FbxVector2 uv_in;
						const char* uvset_name = uvs.GetStringAt(0);
						bool has_uvs;

						bool uvmapped = currentMesh->GetPolygonVertexUV(j, k, mesh->uvsets[u].name.data(), uv_in, has_uvs);
						

						if (uvmapped) {
							//vertex.numberOfUVs++;
							UV uv_out;
							//uv_out.uvset_id = mesh->uvsets[u].id;
							uv_out.U = uv_in[0];
							uv_out.V = uv_in[1];

							//vertex.uvs.push_back(uv_out);
							mesh->uvs.push_back(uv_out);

						}

					}

					//Export blending data
					for (int i = 0; i < mControlPoints[iControlPointIndex]->mBlendingInfo.size(); i++)
					{
						vertex.boneIndex[i] = mControlPoints[iControlPointIndex]->mBlendingInfo[i].mBlendingIndex;
						vertex.boneWeights[i] = mControlPoints[iControlPointIndex]->mBlendingInfo[i].mBlendingWeight;
					}

					mesh->verts.push_back(vertex);
				}
			}
			
			mesh->name = currentMesh->GetNameOnly();
			mesh->header.meshNameLength = mesh->name.length();
			
			// TODO //
			mesh->header.numberOfIndex = 0;
			mesh->header.numberOfVerts = mesh->verts.size();
			//////////

			// SKELETON/JOINT DATA //
			mesh->header.numberOfJoints = joints.size();
			mesh->skeletonHierarchy = joints;

			//Export materials

			int matCount = pCurrentNode->GetMaterialCount();
		
			for (int matID = 0; matID < matCount; matID++) {
				sMaterial* tmp_mat = new sMaterial();

				FbxSurfaceMaterial* mat = (FbxSurfaceMaterial*)pCurrentNode->GetMaterial(0);
				FbxProperty diffuse_property = mat->FindProperty(mat->sDiffuse);
				
				FbxObject* obj = diffuse_property.GetSrcObject();
				FbxTexture* diffuse_tex = obj && obj->Is<FbxTexture>() ? (FbxTexture*)obj : 0;

				if (diffuse_tex != nullptr) {
					FbxFileTexture* diffuse_tex_file = diffuse_tex && diffuse_tex->Is<FbxFileTexture>() ? (FbxFileTexture*)diffuse_tex : 0;
					const char* file_path = diffuse_tex_file->GetFileName();
					tmp_mat->diffuse_path = file_path;
					tmp_mat->data.diffusePathLength = tmp_mat->diffuse_path.size();
				}

				
				tmp_mat->name = std::string(mat->GetName());
				tmp_mat->subheader.matNameLength = tmp_mat->name.length();

				FbxSurfaceLambert* lambert_mat = mat && mat->Is<FbxSurfaceLambert>() ? (FbxSurfaceLambert*)mat : 0;
				if (lambert_mat) {
					FbxDouble3 diffuse = lambert_mat->Diffuse;
					
					//Pack colors
					uint32_t diffuse_r = 255 * diffuse.mData[0];
					uint32_t diffuse_g = 255 * diffuse.mData[1];
					uint32_t diffuse_b = 255 * diffuse.mData[2];

					tmp_mat->data.diffuse |= diffuse_r << 16;
					tmp_mat->data.diffuse |= diffuse_g << 8;
					tmp_mat->data.diffuse |= diffuse_b;
				}

				outMaterials.push_back(tmp_mat);
			}
		}
	}


}

void FBXImporter::ImportStaticMesh(const char * filename, sMesh * mesh, vector<sMaterial*>& outMaterials)
{

	importer->Initialize(filename, -1, manager->GetIOSettings());
	scene = FbxScene::Create(manager, "Scene");

	importer->Import(scene);

	//done with importer
	importer->Destroy();

	FbxNode* pFbxRootNode = scene->GetRootNode();

	//Read bone hierarchy
	std::vector<Joint> joints;
	DumpRecursive(pFbxRootNode, joints, 0, -1);

	if (pFbxRootNode)
	{
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pCurrentNode = pFbxRootNode->GetChild(i);
			if (pCurrentNode->GetNodeAttribute() == NULL) continue;

			FbxNodeAttribute::EType AttributeType = pCurrentNode->GetNodeAttribute()->GetAttributeType();
			if (AttributeType != FbxNodeAttribute::eMesh) continue;

			std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
			ProcessControlPoints(pCurrentNode, mControlPoints);

			FbxMesh* currentMesh = (FbxMesh*)pCurrentNode->GetNodeAttribute();



			if (!currentMesh) continue; //Just in case

			FbxVector4* pVertices = currentMesh->GetControlPoints();


			//Export UVs
			FbxStringList uvs;
			currentMesh->GetUVSetNames(uvs);
			mesh->header.numberOfUVSets = 0;
			for (int i = 0; i < uvs.GetCount(); i++) {
				mesh->header.numberOfUVSets++;

				UVSet uvset;
				uvset.name = uvs.GetStringAt(0);
				uvset.name_length = uvset.name.length();
				uvset.id = i;

				mesh->uvsets.push_back(uvset);
			}

			FbxDeformer *deformer = nullptr;

			int deformer_count = currentMesh->GetDeformerCount();
			for (int d = 0; d < deformer_count; d++) {
				deformer = currentMesh->GetDeformer(d);
				FbxDeformer::EDeformerType deformer_type = deformer->GetDeformerType();

				if (deformer_type == FbxDeformer::eSkin) break;
			}


			FbxSkin *skin = deformer && deformer->Is<FbxSkin>() ? (FbxSkin*)deformer : 0;

			if (skin) {
				int n_of_clusters = skin->GetClusterCount();

				for (int c = 0; c < n_of_clusters; c++) {
					FbxCluster *cluster = skin->GetCluster(c);

					FbxCluster::ELinkMode cluster_mode = cluster->GetLinkMode();

					FbxAMatrix model_to_pose_mat;
					model_to_pose_mat = cluster->GetTransformLinkMatrix(model_to_pose_mat);
					FbxAMatrix model_to_pose_mat_inv = model_to_pose_mat.Inverse();


					FbxAMatrix bone_init_pose;
					bone_init_pose = cluster->GetTransformMatrix(bone_init_pose);

					FbxAMatrix bind_pose_inv = model_to_pose_mat_inv * bone_init_pose;
					FbxAMatrix bild_pose = bind_pose_inv.Inverse();


					int n_of_indices = cluster->GetControlPointIndicesCount();
					int *indices = cluster->GetControlPointIndices();
					double *weights = cluster->GetControlPointWeights();

					for (int ind = 0; ind < n_of_indices; ind++) {
						float weight = (float)weights[ind];

						CtrlPoint *ctrl = new CtrlPoint();
						BlendingIndexWeightPair currBlendingIndexWeightPair;
						//currBlendingIndexWeightPair.mBlendingIndex = currJointIndex;
						//currBlendingIndexWeightPair.mBlendingWeight = currCluster->GetControlPointWeights()[i];

						ctrl->mBlendingInfo.push_back(currBlendingIndexWeightPair);
						mControlPoints[indices[ind]];
					}

				}
			}


			for (int j = 0; j < currentMesh->GetPolygonCount(); j++)
			{
				int vertCount = currentMesh->GetPolygonSize(j);
				assert(vertCount == 3);

				for (int k = 0; k < vertCount; k++)
				{
					int iControlPointIndex = currentMesh->GetPolygonVertex(j, k);
					CtrlPoint* currCtrlPoint = mControlPoints[iControlPointIndex];
					//Export vertices
					Vertex vertex;
					
					vertex.posX = (float)pVertices[iControlPointIndex].mData[0];
					vertex.posY = (float)pVertices[iControlPointIndex].mData[1];
					vertex.posZ = (float)pVertices[iControlPointIndex].mData[2];

					//Export normals
					FbxVector4 normal;
					currentMesh->GetPolygonVertexNormal(j, k, normal);
					vertex.norX = normal[0];
					vertex.norY = normal[1];
					vertex.norZ = normal[2];


					//Export UVs
					FbxStringList uvs;
					currentMesh->GetUVSetNames(uvs);

					for (int u = 0; u < uvs.GetCount(); u++) {

						FbxVector2 uv_in;
						const char* uvset_name = uvs.GetStringAt(0);
						bool has_uvs;

						bool uvmapped = currentMesh->GetPolygonVertexUV(j, k, mesh->uvsets[u].name.data(), uv_in, has_uvs);


						if (uvmapped) {
							//vertex.numberOfUVs++;
							UV uv_out;
							//uv_out.uvset_id = mesh->uvsets[u].id;
							uv_out.U = uv_in[0];
							uv_out.V = uv_in[1];

							//vertex.uvs.push_back(uv_out);
							mesh->uvs.push_back(uv_out);

						}

					}



					mesh->verts.push_back(vertex);
				}
			}

			mesh->name = currentMesh->GetNameOnly();
			mesh->header.meshNameLength = mesh->name.length();

			// TODO //
			mesh->header.numberOfIndex = 0;
			mesh->header.numberOfVerts = mesh->verts.size();
			//////////


			//Export materials

			int matCount = pCurrentNode->GetMaterialCount();

			for (int matID = 0; matID < matCount; matID++) {
				sMaterial* tmp_mat = new sMaterial();

				FbxSurfaceMaterial* mat = (FbxSurfaceMaterial*)pCurrentNode->GetMaterial(0);
				FbxProperty diffuse_property = mat->FindProperty(mat->sDiffuse);

				FbxObject* obj = diffuse_property.GetSrcObject();
				FbxTexture* diffuse_tex = obj && obj->Is<FbxTexture>() ? (FbxTexture*)obj : 0;

				if (diffuse_tex != nullptr) {
					FbxFileTexture* diffuse_tex_file = diffuse_tex && diffuse_tex->Is<FbxFileTexture>() ? (FbxFileTexture*)diffuse_tex : 0;
					const char* file_path = diffuse_tex_file->GetFileName();
					tmp_mat->diffuse_path = file_path;
					tmp_mat->data.diffusePathLength = tmp_mat->diffuse_path.size();
				}


				tmp_mat->name = std::string(mat->GetName());
				tmp_mat->subheader.matNameLength = tmp_mat->name.length();

				FbxSurfaceLambert* lambert_mat = mat && mat->Is<FbxSurfaceLambert>() ? (FbxSurfaceLambert*)mat : 0;
				if (lambert_mat) {
					FbxDouble3 diffuse = lambert_mat->Diffuse;

					//Pack colors
					uint32_t diffuse_r = 255 * diffuse.mData[0];
					uint32_t diffuse_g = 255 * diffuse.mData[1];
					uint32_t diffuse_b = 255 * diffuse.mData[2];

					tmp_mat->data.diffuse |= diffuse_r << 16;
					tmp_mat->data.diffuse |= diffuse_g << 8;
					tmp_mat->data.diffuse |= diffuse_b;
				}

				outMaterials.push_back(tmp_mat);
			}
		}
	}
}

unsigned int FBXImporter::FindJointIndexUsingName(const std::string & inJointName, std::vector<Joint>* joints)
{
	for (unsigned int i = 0; i < joints->size() ; ++i)
	{
		if (joints->at(i).name == inJointName)
		{
			return i;
		}
	}

	throw std::exception("Skeleton information in FBX file is corrupted.");
}

void FBXImporter::ExportBinary(const char * outputFile, sMesh* mesh , vector<sMaterial*>& outMaterials)
{
	std::ofstream file(outputFile, std::ios::binary);
	assert(file.is_open());


	//Write vertex data
	//<MeshHeader meshNameLength=99, numberOfVerts=99, numberOfIndex=99>
	//   <UVSet>
	//      <name>
	//   <UVSet>
	//      <name>
	//   <UVSet>
	//   <Vertex>
	//   <Vertex>
	//   <UV>
	//   <UV>
	//   <UV>

	file.write(reinterpret_cast<char*>(&mesh->header), sizeof(mesh->header));
	file.write((char*)(mesh->name.data()), sizeof(char)*mesh->header.meshNameLength);

	//Export uvsets all at once
	file.write(reinterpret_cast<char*>(mesh->uvsets.data()), sizeof(UVSet) * mesh->uvsets.size());
	//Export uvsets names in the correct order
	for (int i = 0; i < mesh->uvsets.size(); i++) {
		file.write((char*)(mesh->uvsets[i].name.data()), mesh->uvsets[i].name_length);
	}

	//Export vertex data all at once
	file.write(reinterpret_cast<char*>(mesh->verts.data()), sizeof(Vertex) * mesh->header.numberOfVerts);
	//Export uv vertex data in the correct order
	/*
	for (int i = 0; i < mesh->header.numberOfVerts; i++) {
		for (int j = 0; j < mesh->verts[i].numberOfUVs; j++) {
			file.write(reinterpret_cast<char*>(&mesh->uvs[i*mesh->header.numberOfVerts + j]), sizeof(UV));
		}
	}
	*/
	file.write(reinterpret_cast<char*>(mesh->uvs.data()), sizeof(UV) * mesh->header.numberOfVerts * mesh->header.numberOfUVSets);

	//Write material data
	
	//<MatHeader mat_count=4>
	//   <MatSubHeader matNameLength=25>
	//		<name> raw
	//		<sMaterialData diffusePathLength=18>
	//          <diffuse_path> raw
	//   <MatSubHeader>
	//   <MatSubHeader>
	//   ...

	MatHeader materials_info;
	materials_info.mat_count = outMaterials.size();
	
	file.write(reinterpret_cast<char*>(&materials_info), sizeof(MatHeader));

	
	for (int matID = 0; matID < materials_info.mat_count; matID++) {
		sMaterial* material = outMaterials[matID];
		MatSubHeader subheader = material->subheader;
		uint32_t name_length = subheader.matNameLength;

		file.write(reinterpret_cast<char*>(&subheader), sizeof(MatSubHeader));
		file.write((char*)(material->name.data()), subheader.matNameLength * sizeof(char));

		sMaterialData* material_data = &material->data;
		
		file.write(reinterpret_cast<char*>(material_data), sizeof(sMaterialData));

		//Write diffuse texture
		if (material->data.diffusePathLength > 0)
			file.write((char*)(material->diffuse_path.data()), material->data.diffusePathLength * sizeof(char));

	}

	

	file.close();
}
void FBXImporter::ExportSkinnedBinary(const char * outputFile, sSkinnedMesh* mesh, vector<sMaterial*>& outMaterials)
{
	std::ofstream file(outputFile, std::ios::binary);
	assert(file.is_open());


	//Write vertex data
	//<MeshHeader meshNameLength=99, numberOfVerts=99, numberOfIndex=99>
	//   <UVSet>
	//      <name>
	//   <UVSet>
	//      <name>
	//   <UVSet>
	//   <Vertex>
	//   <Vertex>
	//   <UV>
	//   <UV>
	//   <UV>

	// ORDER1 header 
	file.write(reinterpret_cast<char*>(&mesh->header), sizeof(mesh->header));

	// ORDER2 name (actual string)
	file.write((char*)(mesh->name.data()), sizeof(char)*mesh->header.meshNameLength);

	// ORDER3 uvsets
	//Export uvsets all at once
	file.write(reinterpret_cast<char*>(mesh->uvsets.data()), sizeof(UVSet) * mesh->uvsets.size());

	// ORDER4 uvset names
	//Export uvsets names in the correct order
	for (int i = 0; i < mesh->uvsets.size(); i++) {
		file.write((char*)(mesh->uvsets[i].name.data()), mesh->uvsets[i].name_length);
	}

	int _5 = 5;
	file.write(reinterpret_cast<char*>(&_5), sizeof(int));
	///TODO: SkinnedVertex
	// ORDER5 Vertices
	//Export vertex data all at once
	file.write(reinterpret_cast<char*>(mesh->verts.data()), sizeof(SkinnedVertex) * mesh->header.numberOfVerts);
	
	/*
	for (int i = 0; i < mesh->header.numberOfVerts; i++) {
	for (int j = 0; j < mesh->verts[i].numberOfUVs; j++) {
	file.write(reinterpret_cast<char*>(&mesh->uvs[i*mesh->header.numberOfVerts + j]), sizeof(UV));
	}
	}
	*/
	int _6 = 6;
	file.write(reinterpret_cast<char*>(&_6), sizeof(int));
	// ORDER6
	//Write skeleton
	for (auto& joint : mesh->skeletonHierarchy)
	{
		DirectX::XMFLOAT4X4 invBndPs;
		DirectX::XMStoreFloat4x4(&invBndPs, joint.inverseBindPose);
		file.write(reinterpret_cast<char*>(invBndPs.m), sizeof(float)*16);		// TODO:CHECK
		file.write(reinterpret_cast<char*>(&joint.parent_id), sizeof(int));		
	}
	int _7 = 7;
	file.write(reinterpret_cast<char*>(&_7), sizeof(int));

	// ORDER7
	//Write animation
	for (int i = 0; i < mesh->header.frameCount; i++)
	{
		for (int j = 0; j < mesh->header.numberOfJoints; j++)
		{
			DirectX::XMFLOAT4 rot;
			DirectX::XMFLOAT3 trans;

			DirectX::XMStoreFloat4(&rot, mesh->animation.m_aSamples[i].m_aJointPose[j].m_rot);
			DirectX::XMStoreFloat3(&trans, mesh->animation.m_aSamples[i].m_aJointPose[j].m_trans);

			file.write(reinterpret_cast<char*>(&rot), sizeof(float) * 4);
			file.write(reinterpret_cast<char*>(&trans), sizeof(float) * 3);
			file.write(reinterpret_cast<char*>(&mesh->animation.m_aSamples[i].m_aJointPose[j].m_scale), sizeof(float)); //currently always 1.0f

		}
	}

	//file.write(reinterpret_cast<char*>(mesh->uvs.data()), sizeof(UV) * mesh->header.numberOfVerts * mesh->header.numberOfUVSets);

	//Write material data

	//<MatHeader mat_count=4>
	//   <MatSubHeader matNameLength=25>
	//		<name> raw
	//		<sMaterialData diffusePathLength=18>
	//          <diffuse_path> raw
	//   <MatSubHeader>
	//   <MatSubHeader>
	//   ...

	MatHeader materials_info;
	materials_info.mat_count = 0; //TODO

	file.write(reinterpret_cast<char*>(&materials_info), sizeof(MatHeader));


	for (int matID = 0; matID < materials_info.mat_count; matID++) {
		sMaterial* material = outMaterials[matID];
		MatSubHeader subheader = material->subheader;
		uint32_t name_length = subheader.matNameLength;

		file.write(reinterpret_cast<char*>(&subheader), sizeof(MatSubHeader));
		file.write((char*)(material->name.data()), subheader.matNameLength * sizeof(char));

		sMaterialData* material_data = &material->data;

		file.write(reinterpret_cast<char*>(material_data), sizeof(sMaterialData));

		//Write diffuse texture
		if (material->data.diffusePathLength > 0)
			file.write((char*)(material->diffuse_path.data()), material->data.diffusePathLength * sizeof(char));

	}



	file.close();
}

void FBXImporter::ImportBinary(const char * inputFile, sMesh* mesh)
{
	/* OLD
	std::ifstream file(inputFile, std::ios::binary);

	assert(file.is_open());
	file.read(reinterpret_cast<char*>(&mesh->header), sizeof(mesh->header));
	file.read((char*)(mesh->name.data()), mesh->header.meshNameLength);
	mesh->verts.clear();
	mesh->verts.resize(mesh->header.numberOfVerts);

	file.read(reinterpret_cast<char*>(mesh->verts.data()), sizeof(Vertex) * mesh->header.numberOfVerts);

	file.close();
	*/
}
