#include <fbxsdk.h>
#include <fstream>
#include "MeshHeader.h"
#include <iostream>
#include <stdlib.h>
#include "FBXImporter.h"
#include <string>
#include "G6Import.h"

#define out std::cout <<
#define endline << std::endl
#define pause system("pause")
#define clear system("clear")

// http://stackoverflow.com/users/550752/graphitemaster
std::string GetFileExtension(const std::string& FileName)
{
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
	return "";
}

void PrintUV(UV uv) {
	out "UV: { " << uv.U << ", " << uv.V << "}" << std::endl;
}

void PrintVertex(Vertex vertex)
{
	out "--------------------" << std::endl;
	out "Position: { " << vertex.posX << ", " << vertex.posY << ", " << vertex.posZ << "}" << std::endl;
	out "Normal: { " << vertex.norX << ", " << vertex.norY << ", " << vertex.norZ << "}" << std::endl;
	//out "UV: { " << vertex.U << ", " << vertex.V << "}" << std::endl;
}

void PrintMesh(const sMesh& mesh)
{
	out "//////////////\nHEADER//////////////" endline;
	out "Name: " << mesh.name endline;
	out "# of V: " << mesh.header.numberOfVerts endline;

	//for (auto& v : mesh.verts) {
	for (int v = 0; v < mesh.header.numberOfVerts; v++) {
		PrintVertex(mesh.verts[v]);
		for (int u = 0; u < mesh.header.numberOfUVSets; u++) {
			PrintUV(mesh.uvs[v*u + u]);

		}
	}

}

int main(int argc, char * argv[]) 
{
	FBXImporter importer;

	string inDefault;
	string outDefault;

	std::ifstream defaults("defaults.txt");
	std::getline(defaults, inDefault);
	std::getline(defaults, outDefault);
	defaults.close();

	string inFile;
	string outFile;

	//Skip first param, first param indicates which function to use 
	//(e.g. ImportAnimatedMesh(), ExportSkinnedBinary() etc)
	for (int i = 1; i < argc; i++)
	{
		string parameter(argv[i]);
		
		if (parameter == "in")
		{
			inFile = argv[i + 1];

			if (inFile == "default")
			{
				string fileName = argv[i + 2];
				inFile = inDefault + fileName;
				i++;
			}
			//skip next
			i++;
		}

		if (parameter == "out")
		{
			outFile = argv[i + 1];

			if (outFile == "default")
			{
				string fileName = argv[i + 2];
				outFile = outDefault + fileName;
				i++;
			}
			//skip next
			i++;
		}

		if (parameter == "skinned")
		{
			sSkinnedMesh mesh;
			vector<sMaterial*> meshMaterials;
			importer.ImportAnimatedMesh(inFile.c_str(), &mesh, meshMaterials);
			importer.ExportSkinnedBinary(outFile.c_str(), &mesh, meshMaterials);

			//EXIT PROGRAM
			return 0;
		}
	}
	inFile = inDefault;
	outFile = outDefault;

	sSkinnedMesh mesh;
	vector<sMaterial*> meshMaterials;
	importer.ImportAnimatedMesh(inFile.c_str(), &mesh, meshMaterials);
	importer.ExportSkinnedBinary(outFile.c_str(), &mesh, meshMaterials);

	G6Import g6importer;
	sSkinnedMesh* tmp_mesh = new sSkinnedMesh();
	//vector<sMaterial*> tmpMaterials;
	g6importer.ImportAnimatedMesh(outFile.c_str(), tmp_mesh);

	//EXIT PROGRAM
	return 0;

	out inFile endline;
	out outFile endline;

	pause;

	out "testing outpath" endline;

	pause;

	std::ofstream file(outFile);
	file << "poop";
	file.close();
	out "Written." endline;
	pause;
	////////////////////////////////////////////////////////
	////// FBXImporter TEST ////////////////////////////////
	////////////////////////////////////////////////////////

	//sSkinnedMesh mesh;
	//vector<sMaterial*> meshMaterials;

	//importer.ImportAnimatedMesh("sphere_bouncing.fbx", &mesh, meshMaterials);

	////PrintMesh(mesh);

	////pause;
	////clear;

	////importer.ExportBinary("ExpImpTest.G6", &mesh);
	//importer.ExportSkinnedBinary("sphere_bouncing.G6Skin", &mesh, meshMaterials);

	//G6Import g6importer;
	//sSkinnedMesh* tmp_mesh = new sSkinnedMesh();
	////vector<sMaterial*> tmpMaterials;
	//g6importer.ImportAnimatedMesh("sphere_bouncing.G6Skin", tmp_mesh);

	//vector<Vertex> newVerts;
	//newVerts.resize(1);

	//importer.ImportBinary("ExpImpTest.G6", &mesh);

	//PrintMesh(mesh);
	/*
	sMesh mesh;
	G6Import::ImportStaticMesh("ExpImpTest.G6", &mesh);
	PrintMesh(mesh);
	*/

	pause;

	//////////////////////////////////////////////////////
	//// BINARY INPUT OUTPUT TEST BELOW //////////////////
	//////////////////////////////////////////////////////


	//std::ofstream outFile("test3.G6", std::ios::binary);

	//Vertex vertices[3]
	//{
	//	{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	//	{ 21.0f, 2.0f, 33.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	//	{ 1.0f, 2.0f, 3.0f, 20.0f, 0.0f, 1.0f, 31.0f, 0.0f }
	//};
	//outFile.write(reinterpret_cast<char*>(vertices), sizeof(Vertex) * 3);
	//outFile.close();
	//Vertex inVerts[3];

	//std::ifstream in("test3.G6", std::ios::binary);
	//in.read(reinterpret_cast<char*>(inVerts), sizeof(Vertex) * 3);

	//for (int i = 0; i < 3; i++)
	//{
	//	std::cout << "Vertex " << i + 1 << std::endl;
	//	PrintVertex(inVerts[i]);
	//}
	//system("pause");
	//in.close();

}
