#define Mesh_H



#ifdef Mesh_H
#define Mesh_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaders/shader_s.h"

#include <string>
#include <vector>

ussing namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
};

class Mesh {
public:
	// mesh data
    vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
	
		this->vertices = vetices;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
	}
	void Draw(Shader& shader);
private:
	// render data
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};



#endif // Mesh_H


