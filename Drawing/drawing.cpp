
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h> //ALWAYS BEFORE gl.h & glfw.h
#include <glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <common/shader.hpp> //Shader configuration
#include <common/controls.hpp>

int main(void)
{
	//Run GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); //4x AA 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //OpenGL ver 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Prevent using old OpenGL

	//Creating new window
	window = glfwCreateWindow(1024, 768, "Drawings", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true; //Needed in core profile 
	//Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	//To be sure we detect ESC
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//Default cursor position
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	//Background color
	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);
	//Enable depth test
	glEnable(GL_DEPTH_TEST);
	//Passes if the incoming depth value is less than the stored depth value.
	glDepthFunc(GL_LESS);
	//Culling faces with normal not towards camera is off (optimalisation is bad! D: )
	glDisable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//Load shaders
	GLuint programID = LoadShaders("Drawing/VertexShader.txt", "Drawing/FragmentShader.txt");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	static const GLfloat g_vertex_buffer_data[] = {

		//Pyramid
		-1.0f, 0.0f, -1.0f,		1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, -1.0f,		1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,		-1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, 1.0f,		-1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, -1.0f,		1.0f, 0.0f, -1.0f,		-1.0f, 0.0f, 1.0f, //Bottom
		1.0f, 0.0f, 1.0f,		1.0f, 0.0f, -1.0f,		-1.0f, 0.0f, 1.0f,

		//Flat pyramid
		-1.0f + 3, 0.0f, -1.0f + 3,		1.0f + 3, 0.0f, -1.0f + 3,		 0.0f + 3, 0.0f, 0.0f + 1,
		1.0f + 3, 0.0f, -1.0f + 3,		1.0f + 3, 0.0f, 1.0f + 3,	 	 0.0f + 5, 0.0f, 0.0f + 3,
		1.0f + 3, 0.0f, 1.0f + 3,		-1.0f + 3, 0.0f, 1.0f + 3,	 	 0.0f + 3, 0.0f, 0.0f + 5,
		-1.0f + 3, 0.0f, 1.0f + 3,		-1.0f + 3, 0.0f, -1.0f + 3,		 0.0f + 1, 0.0f, 0.0f + 3,
		-1.0f + 3, 0.0f, -1.0f + 3,		1.0f + 3, 0.0f, -1.0f + 3,		 -1.0f + 3, 0.0f, 1.0f + 3, //Bottom
		1.0f + 3, 0.0f, 1.0f + 3,		1.0f + 3, 0.0f, -1.0f + 3,		 -1.0f + 3, 0.0f, 1.0f + 3,

		//Cuboid
		0.0f - 4, 1.0f, -1.0f + 5,		-1.0f - 4, -1.0f, -1.0f + 5,	 -1.0f - 4, 1.0f, -1.0f + 5, //1
		0.0f - 4, 1.0f, -1.0f + 5,		0.0f - 4, -1.0f, -1.0f + 5,		 -1.0f - 4, -1.0f, -1.0f + 5,
		0.0f - 4, 1.0f, 1.0f + 5,		0.0f - 4, -1.0f, -1.0f + 5,		 0.0f - 4, 1.0f, -1.0f + 5,	//2
		0.0f - 4, -1.0f, -1.0f + 5,		0.0f - 4, 1.0f, 1.0f + 5,		 0.0f - 4, -1.0f, 1.0f + 5,
		-1.0f - 4, 1.0f, 1.0f + 5,		-1.0f - 4, -1.0f, 1.0f + 5,		 0.0f - 4, -1.0f, 1.0f + 5,	//3
		0.0f - 4, 1.0f, 1.0f + 5,		-1.0f - 4, 1.0f, 1.0f + 5,		 0.0f - 4, -1.0f, 1.0f + 5,
		-1.0f - 4, -1.0f, -1.0f + 5,	-1.0f - 4, -1.0f, 1.0f + 5,		-1.0f - 4, 1.0f, 1.0f + 5,	//4
		-1.0f - 4, -1.0f, -1.0f + 5,	-1.0f - 4, 1.0f, 1.0f + 5,		-1.0f - 4, 1.0f, -1.0f + 5,
		0.0f - 4, 1.0f, 1.0f + 5,		0.0f - 4, 1.0f, -1.0f + 5,		-1.0f - 4, 1.0f, -1.0f + 5, //5
		0.0f - 4, 1.0f, 1.0f + 5,		-1.0f - 4, 1.0f, -1.0f + 5,		-1.0f - 4, 1.0f, 1.0f + 5,
		0.0f - 4, -1.0f, 1.0f + 5,		-1.0f - 4, -1.0f, -1.0f + 5,	 0.0f - 4, -1.0f, -1.0f + 5,	//6
		0.0f - 4, -1.0f, 1.0f + 5,		-1.0f - 4, -1.0f, 1.0f + 5,		-1.0f - 4, -1.0f, -1.0f + 5,
	};

	static const GLfloat g_color_buffer_data[] = {

		//Pyramid
		0.6, 0.6, 0.6,	0.6, 0.6, 0.6,	0.6, 0.6, 0.6,
		0.7, 0.7, 0.7,	0.7, 0.7, 0.7,	0.7, 0.7, 0.7,
		0.8, 0.8, 0.8,	0.8, 0.8, 0.8,	0.8, 0.8, 0.8,
		0.9, 0.9, 0.9,	0.9, 0.9, 0.9,	0.9, 0.9, 0.9,
		0.0, 0.0, 0.0,	0.0, 0.0, 0.0,	0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,	0.0, 0.0, 0.0,	0.0, 0.0, 0.0,

		//Flat pyramid
		0.6, 0.6, 0.6,	0.6, 0.6, 0.6,	0.6, 0.6, 0.6,
		0.7, 0.7, 0.7,	0.7, 0.7, 0.7,	0.7, 0.7, 0.7,
		0.8, 0.8, 0.8,	0.8, 0.8, 0.8,	0.8, 0.8, 0.8,
		0.9, 0.9, 0.9,	0.9, 0.9, 0.9,	0.9, 0.9, 0.9,
		1.0, 1.0, 1.0,	1.0, 1.0, 1.0,	1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,	1.0, 1.0, 1.0,	1.0, 1.0, 1.0,

		//Cuboid
		0.7, 0.7, 0.7,	0.7, 0.7, 0.7,	0.7, 0.7, 0.7,
		0.7, 0.7, 0.7,	0.7, 0.7, 0.7,	0.7, 0.7, 0.7,
		0.6, 0.6, 0.6,	0.6, 0.6, 0.6,	0.6, 0.6, 0.6,
		0.6, 0.6, 0.6,	0.6, 0.6, 0.6,	0.6, 0.6, 0.6,
		0.8, 0.8, 0.8,	0.8, 0.8, 0.8,	0.8, 0.8, 0.8,
		0.8, 0.8, 0.8,	0.8, 0.8, 0.8,	0.8, 0.8, 0.8,
		1.0, 1.0, 1.0,	1.0, 1.0, 1.0,	1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,	1.0, 1.0, 1.0,	1.0, 1.0, 1.0,
		0.9, 0.9, 0.9,	0.9, 0.9, 0.9,	0.9, 0.9, 0.9,
		0.9, 0.9, 0.9,	0.9, 0.9, 0.9,	0.9, 0.9, 0.9,
		0.5, 0.5, 0.5,	0.5, 0.5, 0.5,	0.5, 0.5, 0.5,
		0.5, 0.5, 0.5,	0.5, 0.5, 0.5,	0.5, 0.5, 0.5,
	};

	//Vertexbuffer identify
	GLuint vertexbuffer;

	//Generate 1 buffer and put it in vertexbuffer
	glGenBuffers(1, &vertexbuffer);

	//Declaration that further commands are for vertexbuffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	//Send vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	do{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use our shader
		glUseProgram(programID);

		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		//1st buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		//2nd buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute 1. 
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		//Start drawing triangles
		glDrawArrays(GL_TRIANGLES, 0, 24 * 3); // From vertex 0 -> 24, 3 per one triangle

		glDisableVertexAttribArray(0);

		//Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	//Check ESC press
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

};


