#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "Program.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define NUM_LIGHTS 2
#define NUM_MATERIALS 3

#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

GLFWwindow *window;

Program program;
std::vector<float> posBuff;
std::vector<float> norBuff;
std::vector<float> texBuff;

glm::vec3 eye(0.0f, 0.0f, 4.0f);

int material = 0;

struct materialStruct {
	glm::vec3 ka, kd, ks;
	float s;
} materials[NUM_MATERIALS];

struct lightStruct {
	glm::vec3 position;
	glm::vec3 color;
} lights[NUM_LIGHTS];	

materialStruct material1 = {
	glm::vec3(0.2f, 0.2f, 0.2f), //ka
	glm::vec3(0.8f, 0.7f, 0.7f), //kd
	glm::vec3(1.0f, 1.0f, 1.0f), //ks
	10.0f //s
};

materialStruct material2 = {
	glm::vec3(0.0f, 0.2f, 0.2f), //ka
	glm::vec3(0.5f, 0.7f, 0.2f), //kd
	glm::vec3(0.1f, 1.0f, 0.1f), //ks
	100.0f //s
};


materialStruct material3 = {
	glm::vec3(0.2f, 0.2f, 0.2f), //ka
	glm::vec3(0.1f, 0.3f, 0.9f), //kd
	glm::vec3(0.1f, 1.0f, 0.1f), //ks
	1.0f //s
};


lightStruct light1 = {
	glm::vec3(0.0f, 0.0f, 3.0f), //position
	glm::vec3(0.5f, 0.5f, 0.5f), //color
};

lightStruct light2 = {
	glm::vec3(0.0f, 3.0f, 0.0f), //position
	glm::vec3(0.2f, 0.2f, 0.2f), //color
};

struct Baycentric {
	float alpha = -1;
	float beta = -1;
	float gamma = -1;
	Baycentric(float alpha, float beta, float gamma) : alpha(alpha), beta(beta), gamma(gamma) {};
	bool Inside() {
		if (this->alpha >= 0 && this->alpha <= 1) {
			if (this->beta >= 0 && this->beta <= 1) {
				if (this->gamma >= 0 && this->gamma <= 1) return true;
			}
		}
		return false;
	}
};

Baycentric baycentricCoordinate(float xPos, float yPos, glm::vec3 A, glm::vec3 B, glm::vec3 C) {

	float alpha = (-1 * ((xPos - B.x) * (C.y - B.y)) + (yPos - B.y) * (C.x - B.x)) /
		(-1 * ((A.x - B.x) * (C.y - B.y)) + (A.y - B.y) * (C.x - B.x));

	float beta = (-1 * ((xPos - C.x) * (A.y - C.y)) + (yPos - C.y) * (A.x - C.x)) /
		(-1 * ((B.x - C.x) * (A.y - C.y)) + (B.y - C.y) * (A.x - C.x));

	float gamma = 1 - alpha - beta;

	return Baycentric(alpha, beta, gamma);
}		


void Display()
{		
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	/*glm::mat4 viewModel = inverse(viewMatrix);
	glm::vec3 cameraPostest = glm::vec3(viewModel[3]);
	glm::vec3 cameraPos(viewModel[3]);*/

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, -1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	program.Bind();
	program.SendUniformData(modelMatrix, "model");
	program.SendUniformData(viewMatrix, "view");
	program.SendUniformData(projectionMatrix, "projection");
	program.SendUniformData(eye, "eye");
	program.SendUniformData(material, "material");

	//for now we will not use a for loop for materials
	/*for (int i = 0; i < NUM_MATERIALS; i++) {

		program.SendUniformData(materials[i].ka, (const char*)("materials[" + std::to_string(i) + "].ka").c_str());
		program.SendUniformData(materials[i].kd, (const char*)("materials[" + std::to_string(i) + "].kd").c_str());
		program.SendUniformData(materials[i].ks, (const char*)("materials[" + std::to_string(i) + "].ks").c_str());
		program.SendUniformData(materials[i].s,  (const char*)("materials[" + std::to_string(i) + "].s").c_str());

	}*/

	program.SendUniformData(materials[material].ka, "ka");
	program.SendUniformData(materials[material].kd, "kd");
	program.SendUniformData(materials[material].ks, "ks");
	//program.SendUniformData(glm::vec3(1.0f, 0.0f, 0.0f), "ks");

	////program.SendUniformData(1.0f, "s");

	program.SendUniformData(materials[material].s, "s");


	for (int i = 0; i < NUM_LIGHTS; i++) {
		program.SendUniformData(lights[i].color, (const char *)("lights[" + std::to_string(i) + "].color").c_str());
		program.SendUniformData(lights[i].position, (const char*)("lights[" + std::to_string(i) + "].position").c_str());
	}

	glDrawArrays(GL_TRIANGLES, 0, posBuff.size() / 3);
	program.Unbind();

}

// Keyboard character callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key) 
	{
	case 'q':
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case 'M':
		material = CLAMP(--material, 0, NUM_MATERIALS-1);
		break;
	case 'm':
		material = CLAMP(++material, 0, NUM_MATERIALS - 1);
		break;
	default:
		break;
	}
}


void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}

void LoadModel(char* name)
{
	// Taken from Shinjiro Sueda with slight modification
	std::string meshName(name);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if (!rc) {
		std::cerr << errStr << std::endl;
	}
	else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					if (!attrib.normals.empty()) {
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
					}
					if (!attrib.texcoords.empty()) {
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
}

void Init()
{
	materials[0] = material1;
	materials[1] = material2;
	materials[2] = material3;
	lights[0] = light1;
	lights[1] = light2;



	glfwInit();
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment4 - Dathan Johnson", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	LoadModel("../obj/bunny.obj");
	
	program.SetShadersFileName("../shaders/shader.vert", "../shaders/shader.frag");
	program.Init();
	program.SendAttributeData(posBuff, "vPositionModel");
	program.SendAttributeData(norBuff, "vNormalModel");
}


int main()
{	
	Init();
	while ( glfwWindowShouldClose(window) == 0) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}