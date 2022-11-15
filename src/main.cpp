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
#define NUM_SHADERS 3
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

GLFWwindow *window;


Program programs[NUM_SHADERS-1];
std::vector<float> posBuff;
std::vector<float> norBuff;
std::vector<float> texBuff;

glm::vec3 eye(0.0f, 0.0f, 4.0f);

int material = 0;
//int program = 0;
int shader = 0;

int light = 0;

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
	glm::vec3(0.1f, 0.1f, 0.1f), //ks
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


void Gouraud(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix) {
	

	programs[shader].Bind();
	programs[shader].SendUniformData(modelMatrix, "model");
	programs[shader].SendUniformData(viewMatrix, "view");
	programs[shader].SendUniformData(projectionMatrix, "projection");
	programs[shader].SendUniformData(eye, "eye");
	programs[shader].SendUniformData(material, "material");


	programs[shader].SendUniformData(materials[material].ka, "ka");
	programs[shader].SendUniformData(materials[material].kd, "kd");
	programs[shader].SendUniformData(materials[material].ks, "ks");
	programs[shader].SendUniformData(materials[material].s, "s");

	for (int i = 0; i < NUM_LIGHTS; i++) {
		programs[shader].SendUniformData(lights[i].color, (const char*)("lights[" + std::to_string(i) + "].color").c_str());
		programs[shader].SendUniformData(lights[i].position, (const char*)("lights[" + std::to_string(i) + "].position").c_str());
	}

	glDrawArrays(GL_TRIANGLES, 0, posBuff.size() / 3);
	programs[shader].Unbind();
}

void Phong(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix) {

	programs[shader].Bind();
	programs[shader].SendUniformData(modelMatrix, "model");
	programs[shader].SendUniformData(viewMatrix, "view");
	programs[shader].SendUniformData(projectionMatrix, "projection");
	programs[shader].SendUniformData(eye, "eye");
	programs[shader].SendUniformData(material, "material");


	programs[shader].SendUniformData(materials[material].ka, "ka");
	programs[shader].SendUniformData(materials[material].kd, "kd");
	programs[shader].SendUniformData(materials[material].ks, "ks");
	programs[shader].SendUniformData(materials[material].s, "s");

	for (int i = 0; i < NUM_LIGHTS; i++) {
		programs[shader].SendUniformData(lights[i].color, (const char*)("lights[" + std::to_string(i) + "].color").c_str());
		programs[shader].SendUniformData(lights[i].position, (const char*)("lights[" + std::to_string(i) + "].position").c_str());
	}

	glDrawArrays(GL_TRIANGLES, 0, posBuff.size() / 3);
	programs[shader].Unbind();
}

void Sillhouette(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix) {
	programs[shader].Bind();
	programs[shader].SendUniformData(modelMatrix, "model");
	programs[shader].SendUniformData(viewMatrix, "view");
	programs[shader].SendUniformData(projectionMatrix, "projection");
	programs[shader].SendUniformData(eye, "eye");

	glDrawArrays(GL_TRIANGLES, 0, posBuff.size() / 3);
	programs[shader].Unbind();
}

void ShaderType(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix) {
	switch (shader) {
	case 0:
		//Phong(projectionMatrix, viewMatrix, modelMatrix);

		Gouraud(projectionMatrix, viewMatrix, modelMatrix);

		break;
	case 1:
		Phong(projectionMatrix, viewMatrix, modelMatrix);
		break;
	case 2:
		Sillhouette(projectionMatrix, viewMatrix, modelMatrix);
		break;
	default:
		Gouraud(projectionMatrix, viewMatrix, modelMatrix);
		break;
	}
}


void Display()
{		
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, -1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	ShaderType(projectionMatrix, viewMatrix, modelMatrix);

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
		material--;
		material = CLAMP(material, 0, NUM_MATERIALS-1);
		break;
	case 'm':
		material++;
		material = CLAMP(material, 0, NUM_MATERIALS - 1);
		break;
	case '1':
		//Gouraud
		shader = 0;
		break;
	case '2':
		//Phong
		shader = 1;
		break;
	case '3':
		//Sillhouette
		shader = 2;
		break;
	case 'l':
		//cycle light forward
		light++;
		light = CLAMP(light, 0, NUM_LIGHTS - 1);
		break;
	case 'L':
		light--;
		light = CLAMP(light, 0, NUM_LIGHTS - 1);
		//cycle light backwards
		break;
	case 'x':
		lights[light].position.x++;
		// move light in +x dir
		break;
	case 'X':
		lights[light].position.x--;
		//move light in -x dir
		break;
	case 'y':
		lights[light].position.y++;
		//move light in +y dir
		break;
	case 'Y':
		lights[light].position.y--;
		//move light in -y dir
		break;
	case 'z':
		lights[light].position.z++;
		//move light in +z dir
		break;
	case 'Z':
		lights[light].position.z--;
		//move light in -Z dir
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
	
	std::string vertFilePath;
	std::string fragFilePath;
	for (int i = 0; i < NUM_SHADERS; i++) {
		switch (i) {
		case 0:
			vertFilePath = "../shaders/shaderGouraud.vert";
			fragFilePath = "../shaders/shaderGouraud.frag";
			break;
		case 1:
			vertFilePath = "../shaders/shaderPhong.vert";
			fragFilePath = "../shaders/shaderPhong.frag";
			break;
		case 2:
			vertFilePath = "../shaders/shaderSillhouette.vert";
			fragFilePath = "../shaders/shaderSillhouette.frag";
			break;
		default:
			vertFilePath = "../shaders/shaderGouraud.vert";
			fragFilePath = "../shaders/shaderGouraud.frag";
			break;
		}
		programs[i].SetShadersFileName((char*)vertFilePath.c_str(), (char*)fragFilePath.c_str());
		programs[i].Init();
		programs[i].SendAttributeData(posBuff, "vPositionModel");
		programs[i].SendAttributeData(norBuff, "vNormalModel");
		
	}
	/*vertFilePath = "../shaders/shaderPhong.vert";
	fragFilePath = "../shaders/shaderPhong.frag";
	programs[0].SetShadersFileName((char*)vertFilePath.c_str(), (char*)fragFilePath.c_str());
	programs[0].Init();
	programs[0].SendAttributeData(posBuff, "vPositionModel");
	programs[0].SendAttributeData(norBuff, "vNormalModel");*/
		
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