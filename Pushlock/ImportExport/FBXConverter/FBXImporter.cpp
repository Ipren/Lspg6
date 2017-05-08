#include "FBXImporter.h"
#include <assert.h>
#include "MeshHeader.h"
#include "HelperStructs.h"
#include <fstream>

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
	}

	for (int b = 0; b < current_node->GetChildCount(); b++) {
		FbxNode *child_node = current_node->GetChild(b);

		DumpRecursive(child_node, joints, joints.size(), current);
	}
}

void FBXImporter::Import(const char * filename, sMesh* mesh, vector<sMaterial*>& outMaterials)
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
				//uvset.name = uvs.GetStringAt(0);
				string name = uvs.GetStringAt(0);
				uvset.name_length = name.length();
				uvset.id = i;

				mesh->uvsets.push_back(uvset);
				mesh->uvset_names.push_back(uvs.GetStringAt(0));
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

						bool uvmapped = currentMesh->GetPolygonVertexUV(j, k, mesh->uvset_names[u].data(), uv_in, has_uvs);
						

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
	for (int i = 0; i < mesh->uvset_names.size(); i++) {
		file.write((char*)(mesh->uvset_names[i].data()), mesh->uvsets[i].name_length);
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
