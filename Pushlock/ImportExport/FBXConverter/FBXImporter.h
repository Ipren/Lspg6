#pragma once
#include <fbxsdk.h>
#include <fbxsdk\fileio\fbxiosettings.h>
#include <vector>
#include <unordered_map>
#include "MeshHeader.h"
#include "MatHeader.h"
#include "HelperStructs.h"

using std::vector;
using std::string;
using std::wstring;


class FBXImporter
{
public:
	FBXImporter();
	~FBXImporter();
	void Import(const char * filename, sMesh* mesh, vector<sMaterial*>& outMaterials);
	void ExportBinary(const char * outputFile, sMesh* mesh, vector<sMaterial*>& outMaterials);
	void ImportBinary(const char * inputFile, sMesh* mesh);
private:
	void ProcessControlPoints(FbxNode* inNode, std::unordered_map<unsigned int, CtrlPoint*> &mControlPoints);
	FbxManager* manager;
	FbxIOSettings* ios;
	FbxImporter* importer;
	FbxScene* scene;
};

