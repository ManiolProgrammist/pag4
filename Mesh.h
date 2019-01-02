#ifndef MESH_H
#define	MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <Shader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <../Box.h>
using namespace std;

//Przechowujemy ka¿dy z wymaganych wektorów w strukturze o nazwie Vertex, której mo¿emy u¿yæ 
//do zaindeksowania ka¿dego z atrybutów wierzcho³ków. 


struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};
enum TextureEnum {
	texture_diffuse, texture_specular, texture_normal, texture_height
};
class Mesh {
public:
	GLfloat deltaTime;
	/*  Mesh Data  */
	vector<float> vert;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;
	/*  Functions  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

	
	Mesh(){}
	
	void AddTexture(
	TextureEnum type,
		const char* path) {
		Texture tex;
		tex.id = TextureFromFile(path, "",false);
		switch (type)
		{
		case TextureEnum::texture_diffuse:
			tex.type = "texture_diffuse";
			break;

		case TextureEnum::texture_height:
			tex.type = "texture_height";
			break;

			//...
		case TextureEnum::texture_normal:
			tex.type = "texture_normal";
			break;
		case TextureEnum::texture_specular:
			tex.type = "texture_specular";
			break;
		default:
			break;
		}
		tex.path = path;
		textures.push_back(tex);

	}

	
	void DrawInstances(Shader* shader, glm::mat4 *model, bool &rotating, int amount) {
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		shader->use();
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // aktywuj odpowiedni¹ jednostkê teksturowania przed powi¹zaniem
			// pobierz numer tekstury (N dla diffuse_textureN)
			string number;
			string name = textures[i].type;

			if (name == "texture_diffuse") {
				number = std::to_string(diffuseNr);
				diffuseNr++;
			}
			else if (name == "texture_specular") {
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			}
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream
			//cout << name << number<< endl;
													 // now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);

		}


		// narysuj siatkê
		glBindVertexArray(VAO);
		if (indices.size() != 0)
		{
				glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,amount);
				glActiveTexture(GL_TEXTURE0);
		}
		else {
			glDrawElementsInstanced(GL_POINTS, vert.size(), GL_UNSIGNED_INT,0,amount);
		}
		glBindVertexArray(0);
	}


	// render the mesh
	void Draw(Shader* shader, glm::mat4 *model, bool &isFromFile, bool &rotating, bool &isBlue)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		shader->use();
		if (!isFromFile) {
			shader->setBool("isBlue", isBlue);
		}

		for (unsigned int i = 0; i < textures.size(); i++)
		{
		
		
			glActiveTexture(GL_TEXTURE0 + i); // aktywuj odpowiedni¹ jednostkê teksturowania przed powi¹zaniem
			// pobierz numer tekstury (N dla diffuse_textureN)
			string number;
			string name = textures[i].type;

			if (name == "texture_diffuse") {
				number = std::to_string(diffuseNr);
				diffuseNr++;
			}
			else if (name == "texture_specular") {
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			}
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream
			//cout << name << number<< endl;
													 // now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		
		}

		// narysuj siatkê
		glBindVertexArray(VAO);
		if (indices.size() != 0)
		{
			if (isFromFile) {
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
				glActiveTexture(GL_TEXTURE0);
			}
			else
				glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawArrays(GL_POINTS, 0, vert.size());
		}
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.

	}
	void generatePlaneWithNormals(float a) {
		vector<unsigned int> indexBuffer;
		vector<float> vertBuffer;
		int num_vertices = 4 * 3;
		//prawy górny
		vertBuffer.push_back(a);
		vertBuffer.push_back(0);
		vertBuffer.push_back(a);
		//normals
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);
		//prawy dolny
		vertBuffer.push_back(a);
		vertBuffer.push_back(0);
		vertBuffer.push_back(-a);
		//normals
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);
		//lewy górny
		vertBuffer.push_back(-a);
		vertBuffer.push_back(0);
		vertBuffer.push_back(a);
		//normals
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);
		//lewy dolny
		vertBuffer.push_back(-a);
		vertBuffer.push_back(0);
		vertBuffer.push_back(-a);
		//normals
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);



		indexBuffer.push_back(0);
		indexBuffer.push_back(1);
		indexBuffer.push_back(2);


		indexBuffer.push_back(1);
		indexBuffer.push_back(3);
		indexBuffer.push_back(2);

		vert = vertBuffer;
		indices = indexBuffer;
		//setupMesh2();
		setupMeshForObjectWithLighting();
	}

	void generateTorus(int num_segments, int num_rings, float thickness, float R) {
		vector<unsigned int> indexBuffer;
		vector<float> vertBuffer;
		int num_vertices = (num_rings + 1) * (num_segments + 1);

		const float pi = 3.1415926535f;
		const float r1 = R;
		const float r2 = thickness;
		for (int i = 0, index = 0; i <= num_rings; ++i) {
			for (int j = 0; j <= num_segments; ++j, ++index) {
				// Compute texture coordinates (surface parameters)
				// Note that u is added to j. This creates a spiral pattern, which
				// allows us to draw the entire torus in one triangle strip without
				// using degenerate triangles.
				// (Yet we still need degenerate triangles to avoid texture
				// wrap-around)
				float u = float(i) / num_rings;
				float v = (float(j) + u) / num_segments;

				// Compute angles
				float u_angle = u * 2 * pi;
				float v_angle = v * 2 * pi;

				// Position
				float x = cos(u_angle) * (r1 + cos(v_angle) * r2);
				float y = sin(u_angle) * (r1 + cos(v_angle) * r2);
				float z = sin(v_angle) * r2;
				vertBuffer.push_back(x);
				vertBuffer.push_back(y);
				vertBuffer.push_back(z);
			}
		}

		// Compute torus indices
		for (int i = 0, index = 0; i <= num_vertices; ++i) {
			indexBuffer.push_back(int(i % num_vertices));
			indexBuffer.push_back(int((i + num_segments + 1) % num_vertices));
		}
		//cout << vert.size() << endl;
		vert = vertBuffer;
		indices = indexBuffer;
		setupMesh2();
	}
	void generatePlaneWithNormalsAndTextureCords(float a) {
		vector<unsigned int> indexBuffer;
		vector<float> vertBuffer;
		//0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		//prawy górny
		vertBuffer.push_back(a);
		vertBuffer.push_back(0);
		vertBuffer.push_back(a);
		//normals
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);
		//textCords
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);

		//0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//prawy dolny
		vertBuffer.push_back(a);
		vertBuffer.push_back(0);
		vertBuffer.push_back(-a);
		//normals
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);
		//textureCord
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(1.0f);

		//-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		//lewy górny
		vertBuffer.push_back(-a);
		vertBuffer.push_back(0);
		vertBuffer.push_back(a);
		//normals
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);
		//textureCords
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(0.0f);


		//-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		//lewy dolny
		vertBuffer.push_back(-a);
		vertBuffer.push_back(0);
		vertBuffer.push_back(-a);
		//normals
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);
		vertBuffer.push_back(0.0f);
		//textureCords
		vertBuffer.push_back(0.0f);
		vertBuffer.push_back(1.0f);





		indexBuffer.push_back(0);
		indexBuffer.push_back(1);
		indexBuffer.push_back(2);


		indexBuffer.push_back(1);
		indexBuffer.push_back(3);
		indexBuffer.push_back(2);

		vert = vertBuffer;
		indices = indexBuffer;
		//setupMesh2();
		setupMeshForObjectWithLighting();

	}
	void generateBoxWithNormalsAndTextureCords() {
		float verticesBuff[] = {
			// positions          // normals           // texture coords
			//1
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			//2
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			//1
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			//2
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,



			//1
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			//2
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			
		
		
			 //1
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		//2
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,



		


			//1
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 //2
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,


			 //1
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 //2
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f
			


		};

		vert = std::vector<float>(std::begin(verticesBuff), std::end(verticesBuff));
		for (int i = 0;i < 36;i++) {
			indices.push_back(i);
		}
		setupMeshForObjectWithLighting();
	}
	void generateBox(float x, float y, float z, float d) {
		vector<unsigned int> indexBuffer;
		vector<float> vertBuffer;

		//front
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z - d / 2);



		//prawo dó³	
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z - d / 2);



		//lewo dó³
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z - d / 2);



		//lewo góra
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z - d / 2);





		//back
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z + d / 2);



		//prawo dó³	
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z + d / 2);



		//lewo dó³
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z + d / 2);



		//lewo góra
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z + d / 2);







		//left
		//prawo góra
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z - d / 2);


		//prawo dó³
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z - d / 2);


		//lewo dó³
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z + d / 2);

		//lewo góra
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z + d / 2);



		//right
			//prawo góra
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z - d / 2);


		//prawo dó³
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z - d / 2);


		//lewo dó³
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z + d / 2);

		//lewo góra
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z + d / 2);



		//down
		//prawo góra
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z + d / 2);


		//prawo dó³
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z - d / 2);

		//lewo dó³
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z - d / 2);

		//lewo góra
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y - d / 2);
		vertBuffer.push_back(z + d / 2);




		//up


			//prawo góra
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z + d / 2);


		//prawo dó³
		vertBuffer.push_back(x + d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z - d / 2);

		//lewo dó³
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z - d / 2);

		//lewo góra
		vertBuffer.push_back(x - d / 2);
		vertBuffer.push_back(y + d / 2);
		vertBuffer.push_back(z + d / 2);



		indexBuffer.push_back(0);
		indexBuffer.push_back(1);
		indexBuffer.push_back(3);

		indexBuffer.push_back(1);
		indexBuffer.push_back(2);
		indexBuffer.push_back(3);
		for (int i = 6;i < vertBuffer.size();i++) {
			indexBuffer.push_back(indexBuffer[i - 6] + 4);
		}



		vert = vertBuffer;
		indices = indexBuffer;
		setupMesh2();

	}

	void generateSphere(int max, double r)

	{
		vector<unsigned int> indexBuffer;
		vector<float> vertBuffer;
		vector<float> vertBuffer2;
		int ai, fj, maxai, maxfj;

		double x, y, z, x1, y1, z1, fi, alf, dfi, dalf;




		fi = 0.0;

		alf = 0.0;

		maxai = max; maxfj = max;

		dalf = 3.14f / float(maxai);

		dfi = 3.14f / float(maxfj);
		int num_vertices = 0;
		for (fj = 0; fj <= maxfj; fj++)
		{
			for (ai = 0; ai <= maxai + 1; ai++)

			{
				x = r * cos(fi)*cos(alf);

				y = r * sin(fi);

				z = r * cos(fi)*sin(alf);

				vertBuffer.push_back(x);
				vertBuffer.push_back(y);
				vertBuffer.push_back(z);
				num_vertices += 1;

				x1 = r * cos(fi + dfi)*cos(alf);

				y1 = r * sin(fi + dfi);

				z1 = r * cos(fi + dfi)*sin(alf);
				vertBuffer2.push_back(x);
				vertBuffer2.push_back(y);
				vertBuffer2.push_back(z);
				num_vertices += 1;
				alf = float(ai)*dalf;

			}
			alf = 0.0;

			fi = dfi * float(fj);
		}
		for (int i = 0, index = 0; i <= num_vertices; ++i) {
			indexBuffer.push_back(int(i % num_vertices));
			indexBuffer.push_back(int((i + maxai + 1) % num_vertices));
		}

		for (int i = 0; i < vertBuffer2.size(); i++) {
			vertBuffer.push_back(vertBuffer2[i]);
		}

		vert = vertBuffer;
		indices = indexBuffer;
		setupMesh2();
	}

	static unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
	{
		string filename = string(path);
		if (directory != "") {
			filename = directory + '/' + filename;
		}
	

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	//setup mesh for OBJ
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.

		/*przyk³ad : Vertex vertex;
		vertex.Position = glm::vec3(0.2f, 0.4f, 0.6f);
		vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
		vertex.TexCoords = glm::vec2(1.0f, 0.0f);*/
		// = [0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f];
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex normals

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(1);
		// vertex texture coords

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(2);
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		glBindVertexArray(0);
	}

	void setupMesh2()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//cout << "Liczba bajtów w buforze " << vert.size() * sizeof(float) << endl;
		// set the vertex attribute pointers
		// vertex Positions

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//cout << "Stride: " << 3 * sizeof(float) << endl;

		glBindVertexArray(0);
	}
	//setup mesh for generated object with normals
	void setupMeshForObjectWithLighting() {
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);
		
		if (indices.size() > 0) {
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		}

		//cout << "Liczba bajtów w buforze " << vert.size() * sizeof(float) << endl;


		// set the vertex attribute pointers
		// vertex Positions
	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//normalne
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//od tekstury
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}
};


#endif