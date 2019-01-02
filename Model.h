#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <..\Mesh.h>
//#include <Shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;
//unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
class Model
{
private:
	glm::mat4* Transform;
	vector<Texture> textures_loaded;
	glm::mat4* modelMatrices;
	string directory;
	Shader* shader;
	bool isFromFile;
	bool instantiate = false;
	float x;
	float y;
	float z;
	/*  Funkcje   */
	void loadModel(string path) 
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
			return;
		}
		//directory = path.substr(0, path.find_last_of('\\')); oryginal
		directory = path.substr(0, path.find_last_of('/'));
		cout << directory << endl;

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene) 
	{
		// przetwórz wszystkie wêz³y siatki (jeœli istniej¹)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// nastêpnie wykonaj to samo dla ka¿dego z jego dzieci
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh* processMesh(aiMesh *mesh, const aiScene *scene)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			// przetwórz pozycje wierzcho³ków, normalne i wspó³rzêdne tekstury
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			if (mesh->mTextureCoords[0]) // czy siatka zawiera wspó³rzêdne tekstury?
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}

		// przetwórz indeksy
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// przetwórz materia³y
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		}

		return new Mesh(vertices, indices, textures);
	} 

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{   // jeœli tekstura nie zosta³a jeszcze za³adowana, za³aduj j¹
				Texture texture;
				texture.id = Mesh::TextureFromFile(str.C_Str(), directory,false);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // dodaj do za³adowanych wektora textures_loaded
			}
		}
		return textures;
	}
public:
	vector<Mesh*> meshes;

	int amount = 1;
	bool rotating;
	bool isBlue,iswhite;
	void setTransform(glm::mat4* matrix) { Transform = matrix; }
	void SetShader(Shader* s) { shader = s; }
	/*  Funkcje   */
	Model(char *path, bool rotate=true)
	{
		rotating = rotate;
		isFromFile = true;
		loadModel(path);
		Transform = new glm::mat4(1);
	}

	Model(Mesh* mesh,bool isBlue=false,bool rotate = true) 
	{
		this->isBlue = isBlue;
		rotating = rotate;
		isFromFile = false;
		meshes.push_back(mesh);
		Transform = new glm::mat4(1);
	}
	//glm::mat4 GetTransform() {
	//	glm::mat4 model = glm::mat4(1.0);
	//	for (int i = 0;i <(int) model.length;i++) {
	//		model[i] = Transform[0][i];
	//	}
	//	return model;
	//}
	bool ChckInstantiate() {
		return instantiate;
	}
	void setInstantiate(glm::mat4* modelMatrices,int amount) {
		this->modelMatrices = modelMatrices;
		this->amount = amount;
		instantiate = true;
		
	}

	void Draw()
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->Draw(shader, Transform, isFromFile,rotating,isBlue);
		}
	}

	void DrawInstantiate() {
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			unsigned int buffer; //z tego buffera nastêpnie przesy³amy do 3/4/5/6 attribArray
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

			// Poniewa¿ mat4 jest w zasadzie 4 razy wiêkszy ni¿ vec4, musimy zarezerwowaæ 4 atrybuty wierzcho³ków dla tej konkretnej macierzy.
			//Poniewa¿ przypisaliœmy jej lokalizacjê 3, kolumny macierzy bêd¹ mia³y po³o¿enia atrybutów wierzcho³ka 3, 4, 5 i 6.

			unsigned int VAO = meshes[i]->VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
			meshes[i]->DrawInstances(shader, Transform, rotating,amount);
		}
	}

};


	

#endif