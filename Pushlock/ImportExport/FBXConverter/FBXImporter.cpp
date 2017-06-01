#include "FBXImporter.h"
#include <assert.h>
#include "MeshHeader.h"
#include "HelperStructs.h"
#include <fstream>

std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

bool CopyDirTo(const wstring& source_folder, const wstring& target_folder)
{
	wstring new_sf = source_folder + L"\\*";
	WCHAR sf[MAX_PATH + 1];
	WCHAR tf[MAX_PATH + 1];

	wcscpy_s(sf, MAX_PATH, new_sf.c_str());
	wcscpy_s(tf, MAX_PATH, target_folder.c_str());

	sf[lstrlenW(sf) + 1] = 0;
	tf[lstrlenW(tf) + 1] = 0;

	SHFILEOPSTRUCTW s = { 0 };
	s.wFunc = FO_COPY;
	s.pTo = tf;
	s.pFrom = sf;
	s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;
	int res = SHFileOperationW(&s);

	return res == 0;
}

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

//Creates joint hierarchy
void DumpRecursive(FbxNode* current_node, std::vector<Joint> &joints, int current, int parent) {
	if (current_node->GetNodeAttribute() && current_node->GetNodeAttribute()->GetAttributeType() && current_node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
		//Joint found
		Joint j;
		j.name = current_node->GetName();
		j.parent_id = parent;
		joints.push_back(j);

	}

	for (int b = 0; b < current_node->GetChildCount(); b++) {
		FbxNode *child_node = current_node->GetChild(b);

		DumpRecursive(child_node, joints, joints.size(), current);
	}
}

//find joint in vector based on joint name
unsigned int FindJointIndexUsingName(const std::string & inJointName, std::vector<Joint>* joints)
{
	for (unsigned int i = 0; i < joints->size(); ++i)
	{
		if (joints->at(i).name == inJointName)
		{
			return i;
		}
	}

	throw std::exception("Skeleton information in FBX file is corrupted.");
}

/*A static mesh contains the following, which is to be exported:
* - a MeshHeader struct, containing information about how many verts and indices the mesh contains, aswell as mesh name length.
* - a vector of Vertex structs, which holds xyz position, xyz normal and uv.
* - a vector of indecies. These are not currently exported.
* - a vector of UVs (float u, float v). This should be redundant as the Vertex struct already holds the UVs.
* - a uint8_t mat_id variable.
* - a bool is_skinned variable. Redundant, since we use different structs for skinned meshes.
* - a string with name of the mesh. Also not currently exported.
* - 
*/
void FBXImporter::ImportStaticMesh(const char * filename, sMesh* mesh, vector<sMaterial*>& outMaterials, vector<sLight*>& outLights, vector<sCamera*>& outCameras)
{

	outLights.clear();
	outMaterials.clear();
	outCameras.clear();

	importer->Initialize(filename, -1, manager->GetIOSettings());
	scene = FbxScene::Create(manager, "Scene");

	importer->Import(scene);
	
	FbxAxisSystem SceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();	//
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::DirectX);						//
	if (SceneAxisSystem != OurAxisSystem)										//
	{																			//
		OurAxisSystem.ConvertScene(scene);										//
	}
	FbxAxisSystem NewSceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();	//

	//done with importer
	importer->Destroy();

	FbxNode* pFbxRootNode = scene->GetRootNode();

	//Read bone hierarchy
	std::vector<Joint> joints;
	DumpRecursive(pFbxRootNode, joints, 0, -1);

	if (pFbxRootNode) {
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pCurrentNode = pFbxRootNode->GetChild(i);
			if (pCurrentNode->GetNodeAttribute() == NULL) continue;


			//Export lights
			FbxLight* light = (FbxLight*)pCurrentNode->GetNodeAttribute();
			FbxNodeAttribute::EType light_type = light->GetAttributeType();

			if (light_type == FbxNodeAttribute::eLight) {
				FbxDouble3 color = light->Color.Get();
				FbxDouble intensity = light->Intensity.Get();

				FbxDouble3 position = pCurrentNode->LclTranslation.Get();
				FbxDouble3 rotation = pCurrentNode->LclRotation.Get();
				FbxDouble3 scale = pCurrentNode->LclScaling.Get();

				FbxLight::EType type = light->LightType.Get();

				sLight* tmp_light = new sLight();
				tmp_light->type = (uint32_t)type;


				tmp_light->r = color[0];
				tmp_light->g = color[1];
				tmp_light->b = color[2];

				tmp_light->intensity = intensity;

				tmp_light->posx = position[0];
				tmp_light->posy = position[1];
				tmp_light->posz = position[2];

				tmp_light->rotx = rotation[0];
				tmp_light->roty = rotation[1];
				tmp_light->rotz = rotation[2];

				tmp_light->scalex = scale[0];
				tmp_light->scaley = scale[1];
				tmp_light->scalez = scale[2];

				outLights.push_back(tmp_light);
			}


			//Export mesh
			FbxNodeAttribute::EType AttributeType = pCurrentNode->GetNodeAttribute()->GetAttributeType();

			//if node is camera
			if (AttributeType == FbxNodeAttribute::eCamera)
			{
				//We do not export the camera to our format, since we do not need them in our game.
				//sCamera camera;
				sCamera* camera = new sCamera();
				FbxCamera* currentCamera = (FbxCamera*)pCurrentNode->GetNodeAttribute();

				FbxDouble3 pos, look, up;
				up = currentCamera->UpVector.Get();
				pos = currentCamera->Position.Get();
				look = currentCamera->InterestPosition.Get();
				
				camera->roll = currentCamera->Roll.Get();
				
				for (int i = 0; i < 3; i++)
				{
					camera->pos[i] = pos.mData[i];
					camera->look[i] = look.mData[i];
					camera->up[i] = up.mData[i];
				}

				outCameras.push_back(camera);
			}

			//From here onward we are only looking for meshes.
			if (AttributeType != FbxNodeAttribute::eMesh) continue;

			std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
			ProcessControlPoints(pCurrentNode, mControlPoints);


			FbxMesh* currentMesh = (FbxMesh*)pCurrentNode->GetNodeAttribute();
			
		

			if (!currentMesh) continue; //Just in case

			FbxVector4* pVertices = currentMesh->GetControlPoints();


			//Export UVs
			vector<UVSet> uvsets;
			vector<string> uvset_names;
			FbxStringList uvs;
			currentMesh->GetUVSetNames(uvs);
			for (int i = 0; i < uvs.GetCount(); i++) {
				UVSet uvset;
				string name = uvs.GetStringAt(0);
				uvset.name_length = name.length();
				uvset.id = i;

				uvsets.push_back(uvset);
				uvset_names.push_back(uvs.GetStringAt(0));
			}

			FbxDeformer *deformer = nullptr;

			int deformer_count = currentMesh->GetDeformerCount();
			for (int d = 0; d < deformer_count; d++) {
				deformer = currentMesh->GetDeformer(d);
				FbxDeformer::EDeformerType deformer_type = deformer->GetDeformerType();

				if (deformer_type == FbxDeformer::eSkin) break;
			}



			FbxSkin *skin = deformer && deformer->Is<FbxSkin>() ? (FbxSkin*)deformer : 0;

			//Process skin data
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

			//Create Vertex structs used by engine using control points
			for (int j = 0; j < currentMesh->GetPolygonCount(); j++)
			{
				int vertCount = currentMesh->GetPolygonSize(j);
				assert(vertCount == 3);

				for (int k = 0; k < vertCount; k++)
				{
					int iControlPointIndex = currentMesh->GetPolygonVertex(j, k);
					
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

						bool uvmapped = currentMesh->GetPolygonVertexUV(j, k, uvset_names[u].data(), uv_in, has_uvs);
						

						if (uvmapped) {


							if (u == 0) { //Export 1st uv set
								vertex.u = uv_in[0];
								vertex.v = uv_in[1];
							}

							//vertex.numberOfUVs++;
							UV uv_out;
							//uv_out.uvset_id = mesh->uvsets[u].id;
							uv_out.U = uv_in[0];
							uv_out.V = uv_in[1];

							//vertex.uvs.push_back(uv_out);
							//mesh->uvs.push_back(uv_out);

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

					//Get absolute file path
					const char* file_path = diffuse_tex_file->GetFileName();
					const char* file_path_rel;
					string abs_file_path = file_path;

					//Get relative file path
					tmp_mat->diffuse_path = FbxPathUtils::GetRelativeFilePath(scene->GetPathToRootDocument() + "FBXConverter", file_path);
					string file_path_str = file_path;

					//TODO: Copy textures to right folder, change paths if needed.
					//CopyDirTo(s2ws("C:\\Users\\Theo\\Desktop\\Workstation\\LitetSpel\\Pushlock\\ImportExport\\FBXConverter\\cube_textured.fbm\\*"), L"C:/Users/Theo/Desktop/Workstation/LitetSpel/Pushlock/Pushlock");

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

void FBXImporter::ImportAnimatedMesh(const char * filename, sSkinnedMesh* mesh, vector<sMaterial*>& outMaterials)
{
	//Initial setup //////////////////////////////////////////////////
	importer->Initialize(filename, -1, manager->GetIOSettings());	
	scene = FbxScene::Create(manager, "Scene");						
																
	importer->Import(scene);									
																	
	//done with importer				
	importer->Destroy();				

	//Convert to DirectX axis system
	FbxAxisSystem SceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();	
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::DirectX);						
	if (SceneAxisSystem != OurAxisSystem)										
	{																			
		OurAxisSystem.ConvertScene(scene);										
	}																			




	FbxNode* pFbxRootNode = scene->GetRootNode();

	//Read bone hierarchy
	std::vector<Joint> joints;
	{
		DumpRecursive(pFbxRootNode, joints, 0, -1);
		
		//Set root joint parent to -1, as it has no parent
		joints[0].parent_id = -1;
	}


	if (pFbxRootNode)
	{
		int nodes = pFbxRootNode->GetChildCount();

		//For every child of scene root, check attribute.
		//If attribute is of type eMesh, it is a mesh and we process it.
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pCurrentNode = pFbxRootNode->GetChild(i);
			
			//Check for NULL
			if (pCurrentNode->GetNodeAttribute() == NULL) continue;

			//Get type of current node
			FbxNodeAttribute::EType AttributeType = pCurrentNode->GetNodeAttribute()->GetAttributeType();

			//if node is camera
			if (AttributeType == FbxNodeAttribute::eCamera)
			{
				//We do not export the camera to our format, since we do not need them in our game.
				sCamera camera;
				FbxCamera* currentCamera = (FbxCamera*)pCurrentNode->GetNodeAttribute();

				FbxDouble3 pos, look, up;
				up = currentCamera->UpVector.Get();
				pos = currentCamera->Position.Get();
				look = currentCamera->InterestPosition.Get();

				camera.roll = currentCamera->Roll.Get();

				for (int i = 0; i < 3; i++)
				{
					camera.pos[i] = pos.mData[i];
					camera.look[i] = look.mData[i];
					camera.up[i] = up.mData[i];
				}
			}

			

			//From here we are only looking for nodes of eMesh type. If it
			//is of any other type, continue to next child node.
			if (AttributeType != FbxNodeAttribute::eMesh) continue;

			//Found a mesh; process geometry, materials and skin/animation data.
			{
				//Process control points
				//Used for convenience. The map makes it easier to access the correct
				//vertices when processing normals, uvs, skinning data etc.
				std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
				ProcessControlPoints(pCurrentNode, mControlPoints);

				//Cast current node to FbxMesh
				FbxMesh* currentMesh = (FbxMesh*)pCurrentNode->GetNodeAttribute();
				
				///Failsafe
				if (!currentMesh) continue; //Just in case

				FbxVector4* pVertices = currentMesh->GetControlPoints();


				//Export UVs
				vector<UVSet> uvsets;
				vector<string> uvset_names;
				FbxStringList uvs;
				currentMesh->GetUVSetNames(uvs);
				for (int i = 0; i < uvs.GetCount(); i++) {
					UVSet uvset;
					string name = uvs.GetStringAt(0);
					uvset.name_length = name.length();
					uvset.id = i;

					uvsets.push_back(uvset);
					uvset_names.push_back(uvs.GetStringAt(0));
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

				//If current mesh "has a skin", process skinning data
				//and animation data.
				if (skin) {
					int n_of_clusters = skin->GetClusterCount();

					/// ----------------------------------------------------------------------------------------------------------------------
					FbxAnimStack* currAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
					FbxString animStackName = currAnimStack->GetName();
					AnimationName = animStackName.Buffer();
					FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
					FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
					FbxTime end = takeInfo->mLocalTimeSpan.GetStop();


					mesh->animation.m_framesPerSecond = 24;
					mesh->animation.m_frameCount = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
					mesh->animation.m_aSamples.resize(mesh->animation.m_frameCount);

					for (auto& sample : mesh->animation.m_aSamples)
						sample.m_aJointPose.resize(joints.size());
					/// ---------------------------------------------------------------------------------------------------------------------


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
						DirectX::XMStoreFloat4x4(&joints[currJointIndex].inverseBindPose, FbxMatrixToXMFLOAT4X4A(bind_pose_inv));

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

							for (auto& sample : mesh->animation.m_aSamples)
								sample.m_aJointPose.resize(joints.size());
						
						FbxLongLong poop = end.GetFrameCount(FbxTime::eFrames24);
						for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
						{
							//Ghetto solution
							int sampleIndex = i;
							if (i == 0)
								sampleIndex = 0;
							else
								sampleIndex = i - 1;
							FbxTime currTime;
							currTime.SetFrame(i, FbxTime::eFrames24);

							if (mesh->animation.m_aSamples[i-1].m_aJointPose.size() == 0)
								mesh->animation.m_aSamples[i-1].m_aJointPose.resize(joints.size());
							JointPose& currentPose = (mesh->animation.m_aSamples[i-1].m_aJointPose[currJointIndex]);

							FbxAMatrix currentTransformOffset = pCurrentNode->EvaluateGlobalTransform(currTime);
							FbxAMatrix mGlobalTransform = currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateLocalTransform(currTime);

							///DEBUG TEST
							if (currJointName != "joint6")
								int shit = 1;
							///

							//Get rotation
							DirectX::XMFLOAT4 rot(
								mGlobalTransform.GetQ()[0],
								mGlobalTransform.GetQ()[1],
								mGlobalTransform.GetQ()[2],
								mGlobalTransform.GetQ()[3]);

							//Get translation
							DirectX::XMFLOAT3 trans(
								mGlobalTransform.GetT()[0],
								mGlobalTransform.GetT()[1],
								mGlobalTransform.GetT()[2]);

							//Load into currentPose
							currentPose.m_rot = rot;
							currentPose.m_trans = trans;

							//Scale always 1.0f
							currentPose.m_scale = 1.0f;
						}

					}
				}

				//Assign header stuff
				mesh->header.frameCount = mesh->animation.m_frameCount;
				mesh->header.framesPerSecond = mesh->animation.m_framesPerSecond;
				mesh->header.isLooping = mesh->animation.m_isLooping;


				//Here we extract the values from controlpoints in order to
				//get a more usable format for our engine.
				//
				//	struct SkinnedVertex
				//	{
				//		float posX, posY, posZ;
				//		float norX, norY, norZ;
				//		float u, v;
				//		int boneIndex[4];
				//		float boneWeights[4];
				//	};
				for (int j = 0; j < currentMesh->GetPolygonCount(); j++)
				{
					int vertCount = currentMesh->GetPolygonSize(j);
					assert(vertCount == 3);

					for (int k = 0; k < vertCount; k++)
					{
						int iControlPointIndex = currentMesh->GetPolygonVertex(j, k);

						//Assign vertices
						{
							SkinnedVertex vertex;

							//Init all weights and indices
							for (int i = 0; i < 4; i++)
							{
								vertex.boneIndex[i] = -1;
								vertex.boneWeights[i] = 0;
							}

							vertex.posX = (float)pVertices[iControlPointIndex].mData[0];
							vertex.posY = (float)pVertices[iControlPointIndex].mData[1];
							vertex.posZ = (float)pVertices[iControlPointIndex].mData[2];

							//Assign normals
							{
								FbxVector4 normal;
								currentMesh->GetPolygonVertexNormal(j, k, normal);
								vertex.norX = normal[0];
								vertex.norY = normal[1];
								vertex.norZ = normal[2];
							}

							//Export UVs
							FbxStringList uvs;
							currentMesh->GetUVSetNames(uvs);

							for (int u = 0; u < uvs.GetCount(); u++) {


								FbxVector2 uv_in;
								const char* uvset_name = uvs.GetStringAt(0);
								bool has_uvs;

								bool uvmapped = currentMesh->GetPolygonVertexUV(j, k, uvset_names[u].data(), uv_in, has_uvs);


								if (uvmapped) {


									if (u == 0) { //Export 1st uv set
										vertex.u = uv_in[0];
										vertex.v = uv_in[1];
									}

									//vertex.numberOfUVs++;
									UV uv_out;
									//uv_out.uvset_id = mesh->uvsets[u].id;
									uv_out.U = uv_in[0];
									uv_out.V = uv_in[1];

									//vertex.uvs.push_back(uv_out);
									//mesh->uvs.push_back(uv_out);

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


}


void FBXImporter::ExportStaticBinary(const char * outputFile, sMesh* mesh, vector<sMaterial*>& outMaterials, vector<sLight*>& outLights, vector<sCamera*>& outCameras)
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
	//   
	//   <UV>
	//   <UV>
	//   <UV>

	//Step 1: Write MeshHeader struct
	{
		file.write(reinterpret_cast<char*>(&mesh->header), sizeof(mesh->header));
		file.write((char*)(mesh->name.data()), sizeof(char)*mesh->header.meshNameLength);
	}

	/// DONT EXPORT UVSETS, WE USE ONLY ONE 
	////Export uvsets all at once
	//file.write(reinterpret_cast<char*>(mesh->uvsets.data()), sizeof(UVSet) * mesh->uvsets.size());
	////Export uvsets names in the correct order
	//for (int i = 0; i < mesh->uvset_names.size(); i++) {
	//	file.write((char*)(mesh->uvset_names[i].data()), mesh->uvsets[i].name_length);
	//}


	//Step 2: Export vertex data
	///MAKE SURE UVs ARE ASSIGNED TO THE VERTICES
	{
		file.write(reinterpret_cast<char*>(mesh->verts.data()), sizeof(Vertex) * mesh->header.numberOfVerts);
	}

	///TF IS THIS THEN
	//Export uv vertex data in the correct order
	/*
	for (int i = 0; i < mesh->header.numberOfVerts; i++) {
		for (int j = 0; j < mesh->verts[i].numberOfUVs; j++) {
			file.write(reinterpret_cast<char*>(&mesh->uvs[i*mesh->header.numberOfVerts + j]), sizeof(UV));
		}
	}

	*/

	/// DONT EXPORT UV VECTOR, REDUNDANT!
	//file.write(reinterpret_cast<char*>(mesh->uvs.data()), sizeof(UV) * mesh->header.numberOfVerts * mesh->header.numberOfUVSets);
	//

	//Step 3: Write material data
	{
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
	}


	//Write lights
	size_t n_of_lights = outLights.size();
	file.write(reinterpret_cast<const char *>(&n_of_lights), sizeof(size_t));

	file.write(reinterpret_cast<char*>(outLights.data()), sizeof(sLight) * outLights.size());


	//Write cameras
	size_t n_of_cameras = outCameras.size();
	file.write(reinterpret_cast<const char *>(&n_of_cameras), sizeof(size_t));

	file.write(reinterpret_cast<char*>(outCameras.data()), sizeof(sCamera) * outCameras.size());

	//End

	file.close();
}

void FBXImporter::ExportAnimatedBinary(const char * outputFile, sSkinnedMesh * mesh, vector<sMaterial*>& outMaterials)
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

	//Step 1: Write SkinnedMeshHeader struct. We could set bool isLooping here if we know if it will loop the default animation or not.
	{
		file.write(reinterpret_cast<char*>(&mesh->header), sizeof(mesh->header));
		file.write((char*)(mesh->name.data()), sizeof(char)*mesh->header.meshNameLength);
	}

	/// DONT EXPORT UVSETS, WE USE ONLY ONE 
	////Export uvsets all at once
	//file.write(reinterpret_cast<char*>(mesh->uvsets.data()), sizeof(UVSet) * mesh->uvsets.size());
	////Export uvsets names in the correct order
	//for (int i = 0; i < mesh->uvset_names.size(); i++) {
	//	file.write((char*)(mesh->uvset_names[i].data()), mesh->uvsets[i].name_length);
	//}


	//Step 2: Export vertex data
	///MAKE SURE UVs ARE ASSIGNED TO THE VERTICES
	{
		file.write(reinterpret_cast<char*>(mesh->verts.data()), sizeof(SkinnedVertex) * mesh->header.numberOfVerts);
	}

	///TF IS THIS THEN
	//Export uv vertex data in the correct order
	/*
	for (int i = 0; i < mesh->header.numberOfVerts; i++) {
	for (int j = 0; j < mesh->verts[i].numberOfUVs; j++) {
	file.write(reinterpret_cast<char*>(&mesh->uvs[i*mesh->header.numberOfVerts + j]), sizeof(UV));
	}
	}

	*/

	/// DONT EXPORT UV VECTOR, REDUNDANT!
	//file.write(reinterpret_cast<char*>(mesh->uvs.data()), sizeof(UV) * mesh->header.numberOfVerts * mesh->header.numberOfUVSets);
	//

	//Step 3: Write material data
	{
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
	}

	//Step 4: Write animation data
	{
		//Write skeleton
		for (auto& joint : mesh->skeletonHierarchy)
		{
			//DirectX::XMFLOAT4X4 invBndPs;
			//DirectX::XMStoreFloat4x4(&invBndPs, joint.inverseBindPose);
			file.write(reinterpret_cast<char*>(joint.inverseBindPose.m), sizeof(float) * 16);
			file.write(reinterpret_cast<char*>(&joint.parent_id), sizeof(int));
		}

		//Write animation
		for (int i = 0; i < mesh->header.frameCount; i++)
		{
			for (int j = 0; j < mesh->header.numberOfJoints; j++)
			{
				//DirectX::XMFLOAT4 rot;
				//DirectX::XMFLOAT3 trans;

				//DirectX::XMStoreFloat4(&rot, mesh->animation.m_aSamples[i].m_aJointPose[j].m_rot);
				//DirectX::XMStoreFloat3(&trans, mesh->animation.m_aSamples[i].m_aJointPose[j].m_trans);

				file.write(reinterpret_cast<char*>(&mesh->animation.m_aSamples[i].m_aJointPose[j].m_rot), sizeof(float) * 4);
				file.write(reinterpret_cast<char*>(&mesh->animation.m_aSamples[i].m_aJointPose[j].m_trans), sizeof(float) * 3);
				file.write(reinterpret_cast<char*>(&mesh->animation.m_aSamples[i].m_aJointPose[j].m_scale), sizeof(float)); //currently always 1.0f

			}
		}
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
