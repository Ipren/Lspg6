#include "G6Import.h"

bool G6Import::ImportStaticMesh(const char * filename, sMesh * outMesh, vector<sMaterial*>& outMaterials, vector<sLight*>& outLights)
{
	std::ifstream file(filename, std::ios::binary);

	assert(file.is_open());

	//Step 1: Read MeshHeader
	file.read(reinterpret_cast<char*>(&outMesh->header), sizeof(outMesh->header));
	file.read((char*)(outMesh->name.data()), outMesh->header.meshNameLength);
	outMesh->verts.clear();
	outMesh->verts.resize(outMesh->header.numberOfVerts);

	/// NO LONGER USING/IMPORTING/EXPORTING UVSETS
	/*
	outMesh->uvsets.clear();
	outMesh->uvsets.resize(outMesh->header.numberOfUVSets);

	outMesh->uvset_names.clear();
	outMesh->uvset_names.resize(outMesh->header.numberOfUVSets);

	outMesh->uvs.clear();
	outMesh->uvs.resize(outMesh->header.numberOfVerts * outMesh->header.numberOfUVSets);

	//Import UVSets
	file.read(reinterpret_cast<char*>(outMesh->uvsets.data()), sizeof(UVSet) * outMesh->header.numberOfUVSets);

	for (int i = 0; i < outMesh->header.numberOfUVSets; i++) {
		char* uvset_name = (char*)malloc(sizeof(char) * (outMesh->uvsets[i].name_length + 1));
		memset(uvset_name, 0x0, sizeof(char) * (outMesh->uvsets[i].name_length + 1));

		file.read((char*)(uvset_name), outMesh->uvsets[i].name_length* sizeof(char));
		outMesh->uvset_names[i] = string(uvset_name);

		free(uvset_name);
	}
	*/

	// Step 2: Read Vertex data
	file.read(reinterpret_cast<char*>(outMesh->verts.data()), sizeof(Vertex) * outMesh->header.numberOfVerts);

	/// NO LONGER USING/IMPORTING/EXPORTING UV VECTOR
	/*
	file.read(reinterpret_cast<char*>(outMesh->uvs.data()), sizeof(UV) * outMesh->header.numberOfVerts * outMesh->header.numberOfUVSets);
	*/

	/*
	for (int i = 0; i < outMesh->header.numberOfVerts; i++) {
		outMesh->indices.push_back(i);

		int n_of_uvs = outMesh->verts[i].numberOfUVs;
		for (int j = 0; j < n_of_uvs; j++) {
			UV uv;
			file.read(reinterpret_cast<char*>(&uv), sizeof(UV));
			outMesh->verts[i].uvs.push_back(uv);
		}

	}

	*/

	// Step 3: Read materials
	{
		MatHeader materials_header;
		file.read(reinterpret_cast<char*>(&materials_header), sizeof(MatHeader));


		int32_t mat_count = materials_header.mat_count;

		for (int matID = 0; matID < mat_count; matID++) {
			sMaterial* tmp_mat = new sMaterial();
			file.read(reinterpret_cast<char*>(&tmp_mat->subheader), sizeof(MatSubHeader));

			//Read material name
			char* mat_name = (char*)malloc(sizeof(char) * (tmp_mat->subheader.matNameLength + 1));
			memset(mat_name, 0x0, sizeof(char) * (tmp_mat->subheader.matNameLength + 1));
			file.read((char*)(mat_name), tmp_mat->subheader.matNameLength * sizeof(char));
			tmp_mat->name = mat_name;
			free(mat_name);

			file.read((char*)(&tmp_mat->data), sizeof(sMaterialData)); //Fixed size data

			if (tmp_mat->data.diffusePathLength > 0) {
				char* file_path = (char*)malloc(sizeof(char) * (tmp_mat->data.diffusePathLength + 1));
				memset(file_path, 0x0, sizeof(char) * (tmp_mat->data.diffusePathLength + 1));

				file.read((char*)(file_path), tmp_mat->data.diffusePathLength * sizeof(char));
				tmp_mat->diffuse_path = string(file_path);

				free(file_path);
			}

			uint32_t diffuse = tmp_mat->data.diffuse;

			//Unpack colors
			float diff_r = ((diffuse >> 16) & 255) / 255.0;
			float diff_g = ((diffuse >> 8) & 255) / 255.0;
			float diff_b = (diffuse & 255) / 255.0;
		}
	}

	//Read lights
	file.read(reinterpret_cast<char*>(outLights.data()), sizeof(sLight) * outMesh->header.numberOfLights);


	file.close();

	return true;
}

bool G6Import::ImportAnimatedMesh(const char * filename, sSkinnedMesh * outMesh)
{
	std::ifstream file(filename, std::ios::binary);

	assert(file.is_open());

	// Step 1: Read SkinnedMeshHeader data
	{
		file.read(reinterpret_cast<char*>(&outMesh->header), sizeof(outMesh->header));
		file.read((char*)(outMesh->name.data()), outMesh->header.meshNameLength);
	}

	// Step 2: Read vertex data
	{
		outMesh->verts.clear();
		outMesh->verts.resize(outMesh->header.numberOfVerts);

		file.read(reinterpret_cast<char*>(outMesh->verts.data()), sizeof(SkinnedVertex) * outMesh->header.numberOfVerts);
	}

	// Step 3: Read materials 
	{
		MatHeader materials_header;
		file.read(reinterpret_cast<char*>(&materials_header), sizeof(MatHeader));


		int32_t mat_count = materials_header.mat_count;

		for (int matID = 0; matID < mat_count; matID++) {
			sMaterial* tmp_mat = new sMaterial();
			file.read(reinterpret_cast<char*>(&tmp_mat->subheader), sizeof(MatSubHeader));

			//Read material name
			char* mat_name = (char*)malloc(sizeof(char) * (tmp_mat->subheader.matNameLength + 1));
			memset(mat_name, 0x0, sizeof(char) * (tmp_mat->subheader.matNameLength + 1));
			file.read((char*)(mat_name), tmp_mat->subheader.matNameLength * sizeof(char));
			tmp_mat->name = mat_name;
			free(mat_name);

			file.read((char*)(&tmp_mat->data), sizeof(sMaterialData)); //Fixed size data

			if (tmp_mat->data.diffusePathLength > 0) {
				char* file_path = (char*)malloc(sizeof(char) * (tmp_mat->data.diffusePathLength + 1));
				memset(file_path, 0x0, sizeof(char) * (tmp_mat->data.diffusePathLength + 1));

				file.read((char*)(file_path), tmp_mat->data.diffusePathLength * sizeof(char));
				tmp_mat->diffuse_path = string(file_path);

				free(file_path);
			}

			uint32_t diffuse = tmp_mat->data.diffuse;

			//Unpack colors
			float diff_r = ((diffuse >> 16) & 255) / 255.0;
			float diff_g = ((diffuse >> 8) & 255) / 255.0;
			float diff_b = (diffuse & 255) / 255.0;
		}
	}

	//Step 4: Read animation data
	{
		for (int i = 0; i < outMesh->header.numberOfJoints; i++)
		{
			Joint curr;
			curr.globalTransform = DirectX::XMMatrixIdentity();
			curr.localTransform = DirectX::XMMatrixIdentity();
			curr.name = "someName";

			DirectX::XMFLOAT4X4 invBndPs;
			file.read(reinterpret_cast<char*>(invBndPs.m), sizeof(float) * 16);
			file.read(reinterpret_cast<char*>(&curr.parent_id), sizeof(int));

			curr.inverseBindPose = DirectX::XMLoadFloat4x4(&invBndPs);

			outMesh->skeletonHierarchy.push_back(curr);
		}

		for (int sample = 0; sample < outMesh->header.frameCount; sample++)
		{
			AnimationSample currentSample;
			for (int joint = 0; joint < outMesh->header.numberOfJoints; joint++)
			{
				JointPose currentPose;

				DirectX::XMFLOAT4 rot;
				DirectX::XMFLOAT3 trans;

				file.read(reinterpret_cast<char*>(&rot), sizeof(float) * 4);
				file.read(reinterpret_cast<char*>(&trans), sizeof(float) * 3);

				currentPose.m_rot = DirectX::XMLoadFloat4(&rot);
				currentPose.m_trans = DirectX::XMLoadFloat3(&trans);

				file.read(reinterpret_cast<char*>(&currentPose.m_scale), sizeof(float));

				currentSample.m_aJointPose.push_back(currentPose);
			}
			outMesh->animation.m_aSamples.push_back(currentSample);
		}

		//Extra bits//////////////////////////////////////////////////////////////
		outMesh->animation.m_frameCount = outMesh->header.frameCount;			//
		outMesh->animation.m_framesPerSecond = outMesh->header.framesPerSecond;	//
		//////////////////////////////////////////////////////////////////////////

	}


	file.close();

	return true;
}

G6Import::G6Import()
{
}


G6Import::~G6Import()
{
}
