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

int main() 
{
	//////////////////////////////////////////////////////
	//// FBXImporter TEST ////////////////////////////////
	//////////////////////////////////////////////////////

	FBXImporter importer;
	sMesh mesh;
	vector<sMaterial*> meshMaterials;

	importer.Import("tower.fbx", &mesh, meshMaterials);
	PrintMesh(mesh);

	//pause;
	//clear;

	//importer.ExportBinary("ExpImpTest.G6", &mesh);
	importer.ExportBinary("tower.G6", &mesh, meshMaterials);

	G6Import g6importer;
	sMesh* tmp_mesh = new sMesh();
	vector<sMaterial*> tmpMaterials;
	g6importer.ImportStaticMesh("tower.G6", tmp_mesh, tmpMaterials);

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
