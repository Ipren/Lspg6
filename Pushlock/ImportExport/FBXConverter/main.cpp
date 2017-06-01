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
		//for (int u = 0; u < mesh.header.numberOfUVSets; u++) {
		//	PrintUV(mesh.uvs[v*u + u]);

		//}
	}

}

int main() 
{
	//////////////////////////////////////////////////////
	//// FBXImporter TEST ////////////////////////////////
	//////////////////////////////////////////////////////

	FBXImporter importer;
	sSkinnedMesh mesh;
	vector<sMaterial*> meshMaterials;

	importer.ImportAnimatedMesh("C:\\Users\\TheoXD\\Desktop\\char_1.fbx", &mesh, meshMaterials);


	importer.ExportAnimatedBinary("C:\\Users\\TheoXD\\Desktop\\MASTEEEEEEER\\Pushlock\\Pushlock\\char_1.G6Skin", &mesh, meshMaterials);

	//G6Import::ImportStaticMesh("C:/Users/Theo/Desktop/Workstation/LitetSpel/Pushlock/ImportExport/FBXConverter/light.G6Mesh", &mesh, meshMaterials, outLights, outCameras);


	//G6Import g6importer;
	//sSkinnedMesh* tmp_mesh = new sSkinnedMesh();
	//g6importer.ImportAnimatedMesh("IceWall.G6Skin", tmp_mesh);

	pause;
}
