#include "stdafx.h"


#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef struct {
	float x, y, z;
}VertexType;

typedef struct {
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType;

typedef struct {
	int vIndex;
	int tIndex;
	int nIndex;
}FaceElementType;

void GetModelFilename(char*);
bool ReadFileCounts(char*, int&, int&, int&, int&, vector<int>&);
bool LoadDataStructures(char*, int, int, int, int, vector<int>);

int main() {
	bool result;
	char filename[256];
	int vertexCount, textureCount, normalCount, faceCount;
	vector<int> vertexPerFace;
	char garbage;

	GetModelFilename(filename);

	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount, vertexPerFace);
	if (!result) {
		return -1;
	}

	cout << endl;
	cout << "Vertices:	" << vertexCount << endl;
	cout << "UVs :		" << textureCount << endl;
	cout << "Normals :	" << normalCount << endl;
	cout << "Faces :	" << faceCount << endl;

	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount, vertexPerFace);
	if (!result) {
		return -1;
	}

	cout << "\nFile has been converted." << endl;
	cout << "\nDo you want to exit (y/n)?";
	cin >> garbage;

	return 0;
}


void GetModelFilename(char* filename) {
	bool done;
	ifstream fin;

	done = false;
	while (!done) {
		cout << "Enter model filename: ";

		cin >> filename;

		fin.open(filename);

		if (fin.good()) {
			done = true;
		}
		else {
			fin.clear();
			cout << endl;
			cout << "File " << filename << " count not be opened." << endl << endl;
		}
	}

	return;
}


bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount, vector<int>& vertexPerFace) {
	ifstream fin;
	char input;

	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;
	vertexPerFace.clear();
	
	int vertexPerFaceCnt = 0;

	fin.open(filename);

	if (fin.fail() == true)
		return false;

	fin.get(input);
	//eof()�� ������ ������ ���θ� ��ȯ�ϴ� �Լ�
	while (!fin.eof()) {
		if (input == 'v') {
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		// ������ face ������ ���� ��
		if (input == 'f') {
			fin.get(input);
			// face ���� �κ��� ���� ���
			if (input == ' ') { 

				// face �ϳ��� vertex ������ Ȯ��
				while (input != '\n') {
					fin.get(input);
					if (input == ' ') {
						vertexPerFaceCnt++;
					}
				}
				vertexPerFaceCnt++;
				//vertexPerFace ����Ʈ�� face�� vertex ���� �����Ѵ�.
				vertexPerFace.push_back(vertexPerFaceCnt);

				// face ������ �߰�
				// face �� vertex ������ 3�� �ʰ��ϸ� face�� �ﰢ������ ����
				faceCount+= 1+(vertexPerFaceCnt-3);
			}
			// 
			vertexPerFaceCnt = 0;
		}

		while (input != '\n') {
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	return true;
}


bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount, vector<int> vertexPerFace) {
	VertexType* vertices, * texcoords, * normals;
	FaceType* faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;

	vertices = new VertexType[vertexCount];
	if (!vertices) {
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords) {
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals) {
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces) {
		return false;
	}


	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	fin.open(filename);

	if (fin.fail() == true) {
		return false;
	}

	fin.get(input);
	while (!fin.eof()) {
		if (input == 'v') {
			fin.get(input);

			if (input == ' ') {
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;
				// Z ������ ������ ��Ģ���� �޼� ��Ģ���� �ٲ۴�
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			if (input == 't') {
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// V �ؽ��� ��ǥ�� �޼� ��Ģ���� ������Ų��
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			if (input == 'n') {
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Z ������ �������� �޼� �ý������� ����.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		if (input == 'f') {
			fin.get(input);
				if (input == ' ') {
					int excessedvertex = 0;
					vector<FaceElementType> vecFace;
					vecFace.clear();
					FaceElementType face;
					while (input != '\n') {
						if (input == ' ') {
							fin >> face.vIndex >> input2 >> face.tIndex >> input2 >> face.nIndex;
							vecFace.push_back(face);
						}
						fin.get(input);
					}
					excessedvertex = vecFace.size() - 3;
					for (int i = 0; i <= excessedvertex; i++) {
						faces[faceIndex].vIndex3 = vecFace[0].vIndex;
						faces[faceIndex].vIndex2 = vecFace[1 + i].vIndex;
						faces[faceIndex].vIndex1 = vecFace[2 + i].vIndex;

						faces[faceIndex].tIndex3 = vecFace[0].tIndex;
						faces[faceIndex].tIndex2 = vecFace[1 + i].tIndex;
						faces[faceIndex].tIndex1 = vecFace[2 + i].tIndex;

						faces[faceIndex].nIndex3 = vecFace[0].nIndex;
						faces[faceIndex].nIndex2 = vecFace[1 + i].nIndex;
						faces[faceIndex].nIndex1 = vecFace[2 + i].nIndex;
						faceIndex++;
					}
				}
		}

		while (input != '\n') {
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	fout.open("model.txt");

	fout << "Vertex Count: " << (faceCount * 3) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	for (int i = 0; i < faceIndex; i++) {
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;
	}

	fout.close();

	if (vertices) {
		delete[]vertices;
		vertices = 0;
	}

	if (texcoords) {
		delete[] texcoords;
		texcoords = 0;
	}

	if (normals) {
		delete[] normals;
		normals = 0;
	}

	if (faces) {
		delete[] faces;
		faces = 0;
	}

	return true;
}