#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h> //ALWAYS BEFORE gl.h & glfw.h
#include <glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <common/shader.hpp> //Shader configuration
#include <common/controls.hpp>
#include <common/texture.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/text2D.hpp> 

int main(void)
{
	//Run GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); //4x AA 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //Resize blockade
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //OpenGL ver 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Prevent using old OpenGL

	//Creating new window
	window = glfwCreateWindow(1024, 768, "Object", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true; //Needed for core profile
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
	//Culling faces with normal not towards camera is on
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//Load shaders
	GLuint programID = LoadShaders("Object/VertexShader.txt", "Object/FragmentShader.txt");

	//Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	//Load the texture
	GLuint Texture = loadDDS("Object/uvmap.dds");

	//Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	//Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("Object/fennec.obj", vertices, uvs, normals);

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	//Load into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	//Buffer for indices
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	//Handle for "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	//Initialize font library
	initText2D("Object/Holstein.DDS");

	//Reading time
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	//Used in rotations
	float angleX = 0,
		angleY = 0;
	int i = 0;
	do{
		// Speed measurment
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0){
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		//Get the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix *
			rotate(glm::mat4(1.0f), angleX, vec3(1.f, 0.f, 0.f)) *				//Rotations
			rotate(glm::mat4(1.0f), angleY, vec3(0.f, 1.f, 0.f)) * ModelMatrix; //Rotations

		//Rotate by arrows implementation
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) angleX -= .05f;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) angleX -= 0.1f;
			if (angleX <= 0) angleX = 360;

		}

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)angleX += .05f;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) angleX += 0.1f;
			if (angleX >= 360) angleX = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)angleY -= .05f;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) angleY -= 0.1f;
			if (angleY <= 0) angleY = 360;
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)angleY += .05f;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) angleY += 0.1f;
			if (angleY >= 360) angleY = 0;
		}

		//Send our transformation to the currently bound shader
		//in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		//Set light position
		glm::vec3 lightPos = glm::vec3(5, 15, 25);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


		//Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		//Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		//1st buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		//2nd buffer : UV
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		//3rd buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		//Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		//Start drawing triangles
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		//Show 2D text
		char text[256];
		sprintf(text, "%.f sec", glfwGetTime());
		printText2D(text, 10, 500, 60);

		//Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	//Check ESC press
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	//Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &VertexArrayID);

	//Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
