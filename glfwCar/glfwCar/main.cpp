#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include "camera.h"

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);

// window size
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));  // camera position
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


struct PointLight {
	glm::vec3 lightColor;
	glm::vec3 lightPos;
	glm::vec3 lightAmbient;
	glm::vec3 lightDiffuse;
	glm::vec3 lightSpecular;
	float lightConstant;
	float lightLinear;
	float lightQuadratic;
};

struct Material {
	glm::vec3 matAmbient;
	glm::vec3 matDiffuse;
	glm::vec3 matSpecular;
	float matShininess;
};





class Mesh {
public:
    virtual void Render() = 0;
};

class Quad : public Mesh {
public:
	Quad(float sizeX, float sizeY, float tx, float ty) : m_sizeX(sizeX), m_sizeY(sizeY) {
		float vertices[] = {
			-0.5f, -0.5f,  0.f,
			 0.5f, -0.5f,  0.f,
			 0.5f,  0.5f,  0.f,
			 0.5f,  0.5f,  0.f,
			-0.5f,  0.5f,  0.f,
			-0.5f, -0.5f,  0.f,
		};

		float normals[] = {
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
		};

		float textureCoords[] = {
			0.0f, 0.0f,
			tx, 0.0f,
			tx, ty,
			tx, ty,
			0.0f, ty,
			0.0f, 0.0f,
		};


		glGenBuffers(3, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	Quad() : Quad(1.f, 1.f, 10, 5) {
	}

	virtual void Render() override {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}


	float m_sizeX;
	float m_sizeY;
	unsigned int VAO;
	unsigned int VBO[3];
};

class Cube : public Mesh {
public:
	Cube(float size) : m_size(size) {
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};

		float normals[] = {
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,

			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,

			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,


			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
		};

		float textureCoords[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,

			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,

			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,

			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,

			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f
		};


		glGenBuffers(3, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	Cube() : Cube(1.f) {
	}

	virtual void Render() override {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}


	float m_size;
	unsigned int VAO;
	unsigned int VBO[3];
};


class Sphere : public Mesh {
public:
	Sphere(float radius, float sectorCount, float stackCount) : m_radius(radius), m_sectorCount(sectorCount), m_stackCount(stackCount) {
		m_nVertice = (sectorCount + 1) * (stackCount + 1);
		m_nIndices = sectorCount * stackCount * 2 * 3;


		float* vertices = new float[m_nVertice * 3];
		float* normals = new float[m_nVertice * 3];
		float* texCoords = new float[m_nVertice * 2];
		int* indices = new int[m_nIndices];


		float PI = 3.1415926;
		float sectorStep = 2 * PI / sectorCount;
		float stackStep = PI / stackCount;
		float sectorAngle = 0;
		float stackAngle = 0;
		int cnt = 0;
		for (int i = 0; i <= stackCount; ++i) {
			stackAngle = PI / 2 - i * stackStep;
			float xy = radius * cosf(stackAngle);             // r * cos(u)
			float z = radius * sinf(stackAngle);              // r * sin(u)
			for (int j = 0; j <= sectorCount; ++j) {
				sectorAngle = j * sectorStep;

				float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

				// vertex position (x, y, z)
				vertices[cnt * 3 + 0] = x;
				vertices[cnt * 3 + 1] = y;
				vertices[cnt * 3 + 2] = z;

				// normalized vertex normal (nx, ny, nz)
				normals[cnt * 3 + 0] = x / m_radius;
				normals[cnt * 3 + 1] = y / m_radius;
				normals[cnt * 3 + 2] = z / m_radius;

				// vertex tex coord (s, t) range between [0, 1]
				texCoords[cnt * 2 + 0] = (float)j / sectorCount;
				texCoords[cnt * 2 + 1] = (float)i / stackCount;


				cnt += 1;
			}
		}

		// generate CCW index list of sphere triangles
		// k1--k1+1
		// |  / |
		// | /  |
		// k2--k2+1
		int iCnt = 0;
		for (int i = 0; i < stackCount; ++i) {
			float k1 = i * (sectorCount + 1);     // beginning of current stack
			float k2 = k1 + sectorCount + 1;      // beginning of next stack
			for (int j = 0; j < sectorCount; ++j) {
				int k1 = (i + 0) * (sectorCount + 1) + j;
				int k2 = (i + 1) * (sectorCount + 1) + j;

				//k1 = > k2 = > k1 + 1
				indices[iCnt++] = k1;
				indices[iCnt++] = k2;
				indices[iCnt++] = k1 + 1;

				// k1+1 => k2 => k2+1
				indices[iCnt++] = k1 + 1;
				indices[iCnt++] = k2;
				indices[iCnt++] = k2 + 1;

			}
		}


		glGenBuffers(3, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m_nVertice, vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m_nVertice, normals, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * m_nVertice, texCoords, GL_STATIC_DRAW);


		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_nIndices, indices, GL_STATIC_DRAW);


		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}


	Sphere() : Sphere(1.f, 32, 32) {
	}

	virtual void Render() override {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, m_nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


	int m_nVertice;
	int m_nIndices;
	float m_radius;
	int m_sectorCount;
	int m_stackCount;
	unsigned int VAO;
	unsigned int VBO[3];
	unsigned int EBO;
};


class Cylinder : public Mesh {
public:
	Cylinder(float radius, float height, float sectorCount) :
		m_radius(radius), m_height(height), m_sectorCount(sectorCount) {

		m_nVertice = (sectorCount + 1) * 2 + (sectorCount + 1 + 1) * 2;
		m_nIndices = sectorCount * 2 * 3 + sectorCount * 2 * 3 * 2;
		float* vertices = new float[m_nVertice * 3];
		float* normals = new float[m_nVertice * 3];
		float* texCoords = new float[m_nVertice * 2];
		int* indices = new int[m_nIndices];

		// side
		float PI = 3.1415926;
		float sectorStep = 2 * PI / sectorCount;
		float sectorAngle = 0;
		int cnt = 0;
		float zArray[2] = { m_height, 0 };
		for (int i = 0; i < 2; ++i) {
			float z = zArray[i];
			for (int j = 0; j <= sectorCount; ++j) {
				sectorAngle = j * sectorStep;
				float x = m_radius * cos(sectorAngle);             // r * cos(v)
				float y = m_radius * sin(sectorAngle);             // r * sin(v)

				vertices[cnt * 3 + 0] = x;
				vertices[cnt * 3 + 1] = y;
				vertices[cnt * 3 + 2] = z;

				// normalized vertex normal (nx, ny, nz)
				normals[cnt * 3 + 0] = x / m_radius;
				normals[cnt * 3 + 1] = y / m_radius;
				normals[cnt * 3 + 2] = 0.f;

				// vertex tex coord (s, t) range between [0, 1]
				texCoords[cnt * 2 + 0] = (float)j / sectorCount;
				texCoords[cnt * 2 + 1] = z / m_height;


				cnt += 1;
			}
		}

		// top
		{
			vertices[cnt * 3 + 0] = 0;
			vertices[cnt * 3 + 1] = 0;
			vertices[cnt * 3 + 2] = height;
			normals[cnt * 3 + 0] = 0.f;
			normals[cnt * 3 + 1] = 0.f;
			normals[cnt * 3 + 2] = 1.f;
			texCoords[cnt * 2 + 0] = 0.5f;
			texCoords[cnt * 2 + 1] = 0.5f;
			cnt += 1;
		}
		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;
			float x = m_radius * cos(sectorAngle);             // r * cos(v)
			float y = m_radius * sin(sectorAngle);             // r * sin(v)

			vertices[cnt * 3 + 0] = x;
			vertices[cnt * 3 + 1] = y;
			vertices[cnt * 3 + 2] = height;

			// normalized vertex normal (nx, ny, nz)
			normals[cnt * 3 + 0] = 0.f;
			normals[cnt * 3 + 1] = 0.f;
			normals[cnt * 3 + 2] = 1.f;

			// vertex tex coord (s, t) range between [0, 1]
			texCoords[cnt * 2 + 0] = -(x / m_radius) * 0.5f + 0.5f;
			texCoords[cnt * 2 + 1] = -(y / m_radius) * 0.5f + 0.5f;
			cnt += 1;
		}


		// ground
		{
			vertices[cnt * 3 + 0] = 0;
			vertices[cnt * 3 + 1] = 0;
			vertices[cnt * 3 + 2] = 0;
			normals[cnt * 3 + 0] = 0.f;
			normals[cnt * 3 + 1] = 0.f;
			normals[cnt * 3 + 2] = -1.f;
			texCoords[cnt * 2 + 0] = 0.5f;
			texCoords[cnt * 2 + 1] = 0.5f;
			cnt += 1;
		}
		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;
			float x = m_radius * cos(sectorAngle);             // r * cos(v)
			float y = m_radius * sin(sectorAngle);             // r * sin(v)

			vertices[cnt * 3 + 0] = x;
			vertices[cnt * 3 + 1] = y;
			vertices[cnt * 3 + 2] = 0;

			// normalized vertex normal (nx, ny, nz)
			normals[cnt * 3 + 0] = 0.f;
			normals[cnt * 3 + 1] = 0.f;
			normals[cnt * 3 + 2] = -1.f;

			// vertex tex coord (s, t) range between [0, 1]
			texCoords[cnt * 2 + 0] = -(x / m_radius) * 0.5f + 0.5f;
			texCoords[cnt * 2 + 1] = -(y / m_radius) * 0.5f + 0.5f;
			cnt += 1;
		}


		// k1 + 1  k2 + 1
		// k1      k2 
		int iCnt = 0;
		for (int j = 0; j < sectorCount; ++j) {
			int k1 = j;
			int k2 = (sectorCount + 1) + j;
			//k1 = > k2 = > k1 + 1
			indices[iCnt++] = k1;
			indices[iCnt++] = k2;
			indices[iCnt++] = k1 + 1;

			// k1+1 => k2 => k2+1
			indices[iCnt++] = k1 + 1;
			indices[iCnt++] = k2;
			indices[iCnt++] = k2 + 1;
		}


		for (int i = 0; i <= sectorCount; ++i) {
			indices[iCnt++] = (sectorCount + 1) * 2 + 0;
			indices[iCnt++] = (sectorCount + 1) * 2 + i;
			indices[iCnt++] = (sectorCount + 1) * 2 + i + 1;
		}

		for (int i = 0; i <= sectorCount; ++i) {
			indices[iCnt++] = (sectorCount + 1) * 2 + (sectorCount + 1 + 1) + 0;
			indices[iCnt++] = (sectorCount + 1) * 2 + (sectorCount + 1 + 1) + i;
			indices[iCnt++] = (sectorCount + 1) * 2 + (sectorCount + 1 + 1) + i + 1;
		}



		glGenBuffers(3, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m_nVertice, vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m_nVertice, normals, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * m_nVertice, texCoords, GL_STATIC_DRAW);


		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_nIndices, indices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	Cylinder() : Cylinder(1.f, 1, 32) {
	}


	virtual void Render() override {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, m_nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


	int m_nVertice;
	int m_nIndices;
	float m_radius;
	float m_height;
	int m_sectorCount;
	unsigned int VAO;
	unsigned int VBO[3];
	unsigned int EBO;
};





void setMat(const Material& mat, const Shader& shader) {
	shader.setVec3("material.matAmbient", mat.matAmbient);
	shader.setVec3("material.matDiffuse", mat.matDiffuse);
	shader.setVec3("material.matSpecular", mat.matSpecular);
	shader.setFloat("material.matShininess", mat.matShininess);
}

float carMove = 0;
bool carLight = true;
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // load shader
	//Shader basePhongShader("./Shader/phong.vert", "./Shader/phong.frag");
	Shader phongTextureShader("./Shader/phongTexture.vert", "./Shader/phongTexture.frag");

    // texture
	stbi_set_flip_vertically_on_load(true);
    unsigned int textureGrass = loadTexture("resources/textures/grass.jpg");
    unsigned int textureRoad = loadTexture("resources/textures/road.jpg");
	unsigned int texturePhoto = loadTexture("resources/textures/photo.jpg");


	// lighting
	PointLight light[1];
	light[0].lightColor = glm::vec3{ 1, 1, 1 };
	light[0].lightPos = glm::vec3{ 4, 8, 5 };
	light[0].lightAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	light[0].lightDiffuse = glm::vec3{ 1, 1, 1 };
	light[0].lightSpecular = glm::vec3{ 1, 1, 1 };
	light[0].lightConstant = 1.f;
	light[0].lightLinear = 0.05;
	light[0].lightQuadratic = 0;



	// material
	Material matBase;
	matBase.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	matBase.matDiffuse = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matBase.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matBase.matShininess = 32.f;

	Material matYellow;
	matYellow.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	matYellow.matDiffuse = glm::vec3{ 1.0f, 1.0f, 0.0f };
	matYellow.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matYellow.matShininess = 32.f;

	Material matBlack;
	matBlack.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	matBlack.matDiffuse = glm::vec3{ 0.0, 0.0, 0.0 };
	matBlack.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matBlack.matShininess = 32.f;

	Material matChocolate;
	matChocolate.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	matChocolate.matDiffuse = glm::vec3{ 210 / 255.f,  105 / 255.f, 30 / 255.f };
	matChocolate.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matChocolate.matShininess = 32.f;

	Material matGrey;
	matGrey.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	matGrey.matDiffuse = glm::vec3{ 190 / 255.f,  190 / 255.f, 190 / 255.f };
	matGrey.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matGrey.matShininess = 32.f;
	
	Material matGreen;
	matGreen.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	matGreen.matDiffuse = glm::vec3{ 0 / 255.f,  205 / 255.f, 102 / 255.f };
	matGreen.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matGreen.matShininess = 32.f;
	

	Material matBlue;
	matBlue.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	matBlue.matDiffuse = glm::vec3{ 35 / 255.f,  206 / 255.f, 250 / 255.f };
	matBlue.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matBlue.matShininess = 32.f;

	Material matGoldenrod;
	matGoldenrod.matAmbient = glm::vec3{ 0.2, 0.2, 0.2 };
	matGoldenrod.matDiffuse = glm::vec3{ 184 / 255.f, 134 / 255.f,  11 / 255.f };
	matGoldenrod.matSpecular = glm::vec3{ 1.0f, 1.0f, 1.0f };
	matGoldenrod.matShininess = 32.f;
	


	Quad quad;
	Quad roadQuad(1, 1, 5, 1);
	Quad quadPhoto(1, 1, 1, 1);
	Cube cube;
	Sphere sphere;
	Cylinder cylinder;
	Cylinder cylinder1(1, 1, 4);
	Sphere OcSphere(1.0, 8, 8);


	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader* pCurShader = nullptr;

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix(); // determine the camera's position

		pCurShader = &phongTextureShader;
		pCurShader->setMat4("projection", projection);
		pCurShader->setMat4("view", view);
		pCurShader->setVec3("viewPos", camera.Position);
		pCurShader->use();
		pCurShader->setVec3("pointLights[0].lightColor", light[0].lightColor);
		pCurShader->setVec3("pointLights[0].lightPos", light[0].lightPos);
		pCurShader->setVec3("pointLights[0].lightAmbient", light[0].lightAmbient);
		pCurShader->setVec3("pointLights[0].lightDiffuse", light[0].lightDiffuse);
		pCurShader->setVec3("pointLights[0].lightSpecular", light[0].lightSpecular);
		pCurShader->setFloat("pointLights[0].lightConstant", light[0].lightConstant);
		pCurShader->setFloat("pointLights[0].lightLinear", light[0].lightLinear);
		pCurShader->setFloat("pointLights[0].lightQuadratic", light[0].lightQuadratic);


		glm::vec3 carLightPos = glm::vec3(carMove * 2, 0.6, 0.0);
		glm::vec3 carLightDir = glm::vec3(1, -0.2, 0);

		pCurShader->setVec3("carPos", carLightPos);
		pCurShader->setVec3("spotLight[0].position", carLightPos);
		pCurShader->setVec3("spotLight[0].direction", carLightDir);
		pCurShader->setVec3("spotLight[0].ambient", 0.2, 0.2, 0.2);
		pCurShader->setVec3("spotLight[0].diffuse", 1.0f, 1.0f, 0.0f);
		pCurShader->setVec3("spotLight[0].specular", 1.0f, 1.0f, 1.0f);
		pCurShader->setFloat("spotLight[0].constant", 1.0f);
		pCurShader->setFloat("spotLight[0].linear", 0);
		pCurShader->setFloat("spotLight[0].quadratic", 0);
		pCurShader->setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.f)));
		pCurShader->setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(25.f)));
		pCurShader->setBool("spotLight[0].lightOn", carLight);
		// grassland
		{

			pCurShader = &phongTextureShader;
			glm::mat4 grassModel(1.f);
			grassModel = glm::translate(grassModel, glm::vec3(0, -0.01, 0));
			grassModel = glm::rotate(grassModel, glm::radians(-90.f), glm::vec3(1, 0, 0));
			grassModel = glm::scale(grassModel, glm::vec3(50, 25, 1));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureGrass);
			pCurShader->setMat4("model", grassModel);
			pCurShader->setBool("useTexture", true);
			setMat(matBase, *pCurShader);
			quad.Render();
		}

		//road
		{

			pCurShader = &phongTextureShader;
			glm::mat4 roadModel(1.f);
			roadModel = glm::translate(roadModel, glm::vec3(0, -0.009, 0));
			roadModel = glm::rotate(roadModel, glm::radians(-90.f), glm::vec3(1, 0, 0));
			roadModel = glm::scale(roadModel, glm::vec3(50, 7, 1));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureRoad);
			pCurShader->setMat4("model", roadModel);
			pCurShader->setBool("useTexture", true);
			setMat(matBase, *pCurShader);
			roadQuad.Render();

			// yellow line
			pCurShader->setBool("useTexture", false);
			roadModel = glm::mat4(1.f);
			roadModel = glm::translate(roadModel, glm::vec3(0, -0.008, 3.2));
			roadModel = glm::rotate(roadModel, glm::radians(-90.f), glm::vec3(1, 0, 0));
			roadModel = glm::scale(roadModel, glm::vec3(50, 0.3, 1));
			pCurShader->setMat4("model", roadModel);
			setMat(matYellow, *pCurShader);
			quad.Render();

			roadModel = glm::mat4(1.f);
			roadModel = glm::translate(roadModel, glm::vec3(0, -0.008, -3.2));
			roadModel = glm::rotate(roadModel, glm::radians(-90.f), glm::vec3(1, 0, 0));
			roadModel = glm::scale(roadModel, glm::vec3(50, 0.3, 1));
			pCurShader->setMat4("model", roadModel);
			setMat(matYellow, *pCurShader);
			quad.Render();
		}

		// car
		{

			pCurShader = &phongTextureShader;
			pCurShader->setBool("useTexture", false);

			glm::mat4 carModel(1.f);
			carModel = carModel * glm::translate(glm::mat4(1.f), glm::vec3(carMove, 0, 0));
			carModel = carModel * glm::rotate(carModel, glm::radians(0.f), glm::vec3(0, 1, 0));

			// tire
			glm::mat4 model(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(-1.0, 0, 0.8));
			model = glm::translate(model, glm::vec3(0, 0.3, -0.1));
			model = glm::scale(model, glm::vec3(0.3, 0.3, 0.2));
			pCurShader->setMat4("model", model);
			setMat(matBlack, *pCurShader);
			cylinder.Render();
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(-1.0, 0, 0.8));
			model = glm::translate(model, glm::vec3(0, 0.3, -0.105));
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.21));
			pCurShader->setMat4("model", model);
			setMat(matChocolate, *pCurShader);
			cylinder.Render();


			// tire
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(-1.0, 0, -0.8));
			model = glm::translate(model, glm::vec3(0, 0.3, -0.1));
			model = glm::scale(model, glm::vec3(0.3, 0.3, 0.2));
			pCurShader->setMat4("model", model);
			setMat(matBlack, *pCurShader);
			cylinder.Render();
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(-1.0, 0, -0.8));
			model = glm::translate(model, glm::vec3(0, 0.3, -0.105));
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.21));
			pCurShader->setMat4("model", model);
			setMat(matChocolate, *pCurShader);
			cylinder.Render();



			// tire
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(1.0, 0, 0.8));
			model = glm::translate(model, glm::vec3(0, 0.3, -0.1));
			model = glm::scale(model, glm::vec3(0.3, 0.3, 0.2));
			pCurShader->setMat4("model", model);
			setMat(matBlack, *pCurShader);
			cylinder.Render();
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(1.0, 0, 0.8));
			model = glm::translate(model, glm::vec3(0, 0.3, -0.105));
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.21));
			pCurShader->setMat4("model", model);
			setMat(matChocolate, *pCurShader);
			cylinder.Render();


			// tire
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(1.0, 0, -0.8));
			model = glm::translate(model, glm::vec3(0, 0.3, -0.1));
			model = glm::scale(model, glm::vec3(0.3, 0.3, 0.2));
			pCurShader->setMat4("model", model);
			setMat(matBlack, *pCurShader);
			cylinder.Render();
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(1.0, 0, -0.8));
			model = glm::translate(model, glm::vec3(0, 0.3, -0.105));
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.21));
			pCurShader->setMat4("model", model);
			setMat(matChocolate, *pCurShader);
			cylinder.Render();


			// car bottom
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(0, 0.3, 0));
			model = glm::scale(model, glm::vec3(3.0, 0.14, 1.4));
			pCurShader->setMat4("model", model);
			setMat(matGrey, *pCurShader);
			cube.Render();

			// car body
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(0, 0.68, 0));
			model = glm::scale(model, glm::vec3(2.8, 0.6, 1.2));
			pCurShader->setMat4("model", model);
			setMat(matGreen, *pCurShader);
			cube.Render();

			// car body
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(0, 1.3, 0));
			model = glm::scale(model, glm::vec3(1.6, 0.8, 1.2));
			pCurShader->setMat4("model", model);
			setMat(matGreen, *pCurShader);
			cube.Render();


			//  car window
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(-0.4, 1.35, 0));
			model = glm::scale(model, glm::vec3(0.7, 0.6, 1.21));
			pCurShader->setMat4("model", model);
			setMat(matBlue, *pCurShader);
			cube.Render();

			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(0.4, 1.35, 0));
			model = glm::scale(model, glm::vec3(0.7, 0.6, 1.21));
			pCurShader->setMat4("model", model);
			setMat(matBlue, *pCurShader);
			cube.Render();

			// front window
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(0.81, 1.35, 0.0));
			model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(1.1, 0.6, 1));
			pCurShader->setMat4("model", model);
			setMat(matBlue, *pCurShader);
			quad.Render();


			// light
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(1.4, 0.65, 0.38));
			model = glm::scale(model, glm::vec3(0.05, 0.13, 0.13));
			pCurShader->setMat4("model", model);
			setMat(matYellow, *pCurShader);
			sphere.Render();

			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(1.4, 0.65, -0.38));
			model = glm::scale(model, glm::vec3(0.05, 0.13, 0.13));
			pCurShader->setMat4("model", model);
			setMat(matYellow, *pCurShader);
			sphere.Render();

			// top
			
			model = glm::mat4(1.f);
			model = model * carModel;
			model = glm::translate(model, glm::vec3(0, 1.71, 0.0));
			model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1, 0, 0));
			model = glm::scale(model, glm::vec3(1.2, 1, 1));
			pCurShader->setMat4("model", model);
			pCurShader->setBool("useTexture", true);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texturePhoto);
			setMat(matBase, *pCurShader);
			quadPhoto.Render();
		}



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float span = 5.0 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, span);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, span);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, span);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, span);

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		carMove += 0.5 * span ;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		carMove -= 0.5 * span ;
	}


	if (glfwGetKey(window, GLFW_KEY_M ) == GLFW_PRESS) {
		carLight = true;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		carLight = false;
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


// load image
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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