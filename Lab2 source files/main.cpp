#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include "Objects.h"
#include "Shader.h"
#include <iostream>

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
struct drawData {
	GLuint VAO;
	int vertexNum;
	int indiceNum;
};

GLuint VAOs[2];
Shader * shaders[2];

// Shader file paths
const char* vertexFile1 = "../../Lab1/vertexShader1.glsl";
const char* fragFile1 = "../../Lab1/fragShader1.glsl";
const char* fragFile2 = "../../Lab1/fragShader2.glsl";

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
GLuint generateObjectBuffer(std::vector<Vertex> vertices, int index) {
	GLuint numVertices = vertices.size();
	// Generate VAO
	GLuint VAO;
	// Genderate 1 generic buffer object, called VBO
	GLuint VBO;
	glGenVertexArrays(1, &VAO);
 	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	// In OpenGL, we bind (make active) the handle to a target name and then execute commands on that target
	// Buffer will contain an array of vertices 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// After binding, we now fill our object with data, everything in "Vertices" goes to the GPU
	glBufferData(GL_ARRAY_BUFFER, numVertices* sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// if you have more data besides vertices (e.g., vertex colours or normals), use glBufferSubData to tell the buffer when the vertices array ends and when the colors start
	//glBufferSubData (GL_ARRAY_BUFFER, 0, numVertices*3*sizeof(GLfloat), vertices);
	//glBufferSubData (GL_ARRAY_BUFFER, numVertices*3*sizeof(GLfloat), numVertices*4*sizeof(GLfloat), colors);

	VAOs[index] = VAO;

return VBO;
}

void linkCurrentBuffertoShader(GLuint shaderProgramID){
	// find the location of the variables that we will be using in the shader program
	GLuint positionID = glGetAttribLocation(shaderProgramID, "vPosition");
	GLuint colorID = glGetAttribLocation(shaderProgramID, "vColor");
	GLuint texposID = glGetAttribLocation(shaderProgramID, "vTexcoord");
	GLuint normalID = glGetAttribLocation(shaderProgramID, "vNormal");

	// Since we loaded everything into buffer at the same time, we use offsets to access specific elements
	// given size of the object and its subobjects that we want (offset is distance to subobject)
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(positionID);

	glVertexAttribPointer(colorID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(colorID);

	glVertexAttribPointer(texposID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(texposID);

	glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(normalID);

}
#pragma endregion VBO_FUNCTIONS


void display(){
	// clear the colorbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// change color with time
	float timeValue = glutGet(GLUT_ELAPSED_TIME);
	float greenValue = (sin(timeValue/1000.f) / 2.0f) + 0.5f;
	glm::vec4 colorValue = glm::vec4(0.0f, greenValue, 0.0f, 1.0f);
	shaders[0]->setVec4f(colorValue, "uniColor");

	// 3d stuff
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, (float)glutGet(GLUT_ELAPSED_TIME)/1000 * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	glm::mat4 view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	shaders[0]->setMat4fv(projection, "projection");
	shaders[0]->setMat4fv(model, "model");
	shaders[0]->setMat4fv(view, "view");
	
	glUseProgram(shaders[0]->getProgramID());
	glBindVertexArray(VAOs[0]);
	// NB: Make the call to draw the geometry in the currently activated vertex buffer. This is where the GPU starts to work!
	glDrawArrays(GL_TRIANGLES, 0, 12);

	/*
	glUseProgram(shaders[1]);
	glBindVertexArray(VAOs[1]);
	// NB: Make the call to draw the geometry in the currently activated vertex buffer. This is where the GPU starts to work!
	glDrawArrays(GL_TRIANGLES, 0, 3);
	*/

	glutSwapBuffers(); 
	glutPostRedisplay();
}

// Keyboard key handler
void processKeyboardKeys(unsigned char key, int x, int y) {

	float fraction = 0.1f;

	switch (key) {
	// 27 is esc key, if esc then exit the program
	case 27:
		exit(0);
	case 'a':

		break;
	case 'd':

		break;
	case 'w':

		break;
	case 's':

		break;
	default:
		break;
	}
		
}

// Special keyboard key handler
void processSpecialKeys(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_F1:
		 break;
	case GLUT_KEY_F2:
		 break;
	case GLUT_KEY_F3:
		break;
	}
}


void init()
{
	Pyramid * pyramid = new Pyramid();
	// Set up the shaders
	Shader * shaderProgram = new Shader(vertexFile1, fragFile1);
	shaders[0] = shaderProgram;
	// Put the vertices and colors into a vertex buffer object
	generateObjectBuffer(pyramid->vertices, 0);
	// Link the current buffer to the shader
	linkCurrentBuffertoShader(shaders[0]->getProgramID());
	//Object 2
	// Set up yellow shaders
	//GLuint secondShaderID = CompileSecondShaders();
	//shaders[1] = secondShaderID;
	// Put the vertices and colors into a vertex buffer object
	//generateObjectBuffer(vertices2, colors2, 1);
	// Link the current buffer to the shader
	//linkCurrentBuffertoShader(secondShaderID);
}

int main(int argc, char** argv){

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);

	// Tell glut about input handling functions
	glutKeyboardFunc(processKeyboardKeys);
	glutSpecialFunc(processSpecialKeys);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();

	// Begin infinite event loop
	glutMainLoop();
    return 0;
}


