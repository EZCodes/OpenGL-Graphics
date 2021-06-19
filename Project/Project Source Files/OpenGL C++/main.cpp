// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <map>

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"

// Image processing includes
#include "stb_image.h"

// Sound processing library
#include <irrKlang.h>

//Text Rendering
#include <ft2build.h>
#include FT_FREETYPE_H 

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
const char* ANT_BODY = "../../Lab1/ant_models/ant_body2.obj";
const char* ANT_UP_LEFT = "../../Lab1/ant_models/ant_up_left.obj";
const char* ANT_UP_RIGHT = "../../Lab1/ant_models/ant_up_right.obj";
const char* ANT_MID_LEFT = "../../Lab1/ant_models/ant_mid_left.obj";
const char* ANT_MID_RIGHT = "../../Lab1/ant_models/ant_mid_right.obj";
const char* ANT_BOT_LEFT = "../../Lab1/ant_models/ant_bot_left.obj";
const char* ANT_BOT_RIGHT = "../../Lab1/ant_models/ant_bot_right.obj";
const char* PLANE = "../../Lab1/ant_models/plane.obj";
const char* ROCKS = "../../Lab1/ant_models/rocks.obj";
const char* CANDLES = "../../Lab1/ant_models/candles.obj";
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/

// TEXTURES TO LOAD
const char* GRASS_TEXTURE = "../../Lab1/grass.jpg";
const char* ROCK_COLOR_TEXTURE = "../../Lab1/ant_models/stone_01_albedo.jpg";
const char* ROCK_NORMAL_TEXTURE = "../../Lab1/ant_models/stone_01_normal.jpg";

// SKYBOX IMAGES
std::vector<std::string> filenames = {
	"../../Lab1/ant_models/skybox/skybox_right.jpg",
	"../../Lab1/ant_models/skybox/skybox_left.jpg",
	"../../Lab1/ant_models/skybox/skybox_top.jpg",
	"../../Lab1/ant_models/skybox/skybox_bot.jpg",
	"../../Lab1/ant_models/skybox/skybox_mid.jpg",
	"../../Lab1/ant_models/skybox/skybox_right2.jpg",
};

// PARTICLE IMAGES
const char* FIRE_PARTICLE = "../../Lab1/ant_models/fire_particle.jpg";
const char* BLOOD_PARTICLE = "../../Lab1/ant_models/blood_particle.jpg";

// SOUNDS
const char* SPLAT_SOUND = "../../Lab1/ant_models/splat_sound.wav";

// FONTS
const char* OCRA_FONT = "../../Lab1/ant_models/fonts/ocraext.ttf";

#pragma region SimpleTypes
typedef struct mData
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec3> mTangents;
	std::vector<vec3> mBitangents;
	std::vector<vec2> mTextureCoords;
	GLuint VAO;
	GLuint vVBO;
	GLuint nVBO;
} ModelData;

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat s_coefficient;
};
#pragma endregion SimpleTypes

// User Ant Meshes
// We leave them separate from crowd, since we are also controlling this ant movement
// so leaving it as separate global makes it easier to control its behaviour
std::vector<ModelData> meshes;
Material ant_properties;

struct Ant {
	ModelData body;
	ModelData up_left;
	ModelData up_right;
	ModelData bot_left;
	ModelData bot_right;
	ModelData mid_left;
	ModelData mid_right;

	// Leg Rotation
	GLfloat mid_rot = 0.0f;
	GLfloat up_rot = 0.0f;
	GLfloat bot_rot = 0.0f;

	// Leg Speed
	GLfloat leg_speed = 4.0f;

	// Move speed
	GLfloat speed = 0.018f;

	// Leg object offsets (from origin for rotation)
	GLfloat x_off = 2.38f;

	// Rotation
	GLfloat rotate_y = 0.0f;

	// Direction
	glm::vec3 dir = glm::vec3(1.0f, 0.0f, 0.0f);

	// Translation
	glm::vec3 trs = glm::vec3(-15.0f, 0.0f, -15.0f);

	// Scaling
	GLfloat scale_x = 0.5f;
	GLfloat scale_y = 0.5f;
	GLfloat scale_z = 0.5f;

	std::vector<GLuint> vVBOs;
	std::vector<GLuint> nVBOs;

	// Material
	Material mat_properties;

	bool isAlive = true;
};

// Ants for crowd
std::vector<Ant> crowdAnts;

// crowd ant materials
std::vector<Material> crowdMats;

glm::vec3 crowd_center = glm::vec3(10.0f, 0.0f, 10.0f);
glm::vec3 crowd_direction = glm::vec3(-1.0f, 0.0f, -1.0f);
GLfloat crowd_speed = 0.019f;

using namespace std;

const int ANT_NUMBER = 5;

// Shaders
GLuint shaderProgramID;
GLuint textureShaderProgID;
GLuint crowdShaderID;
GLuint skyboxShaderID;
GLuint particleShaderID;
GLuint textShaderID;

// Textures
GLuint grass_texture;
GLuint gr_planeVBO;
GLuint rock_color;
GLuint rock_cVBO;
GLuint rock_normals;
GLuint rock_nVBO;

// Skybox
GLuint skybox_texture;
GLuint skyboxVAO, skyboxVBO;

int width = 800;
int height = 600;

GLuint loc1, loc2, loc3;

// Leg Rotation
GLfloat mid_rota = 0.0f;
GLfloat up_rota = 0.0f;
GLfloat bot_rota = 0.0f;

// Leg object offsets (from origin for rotation)
GLfloat x_off = 2.38f;

// Leg Speed
GLfloat leg_speed = 5.0f;

// Rotation
GLfloat rotate_y = 90.0f;
GLfloat rotate_x = 0.0f;
GLfloat rotate_z = 0.0f;

// Translation
GLfloat trs_x = 0.0f;
GLfloat trs_y = 0.0f;
GLfloat trs_z = 0.0f;

// Scaling
GLfloat scale_x = 1.0f;
GLfloat scale_y = 1.0f;
GLfloat scale_z = 1.0f;

// Collision borders

// ant-crowd distance between middles
GLfloat body_lengths = 4.5f;

// map edges
GLfloat x_border = 35.5f;
GLfloat z_border = 35.5f;
// middle rocks
std::vector<glm::vec3> collision_points = {
	// rock 1
	glm::vec3(6.8f, 0.0f, -15.9f),
	glm::vec3(17.75f, 0.0f, -20.0f),
	glm::vec3(18.0f, 0.0f, -12.5f),
	glm::vec3(11.24f, 0.0f, -21.04f),
	// rock 2
	glm::vec3(-6.35f, 0.0f, 5.5f),
	glm::vec3(-19.2f, 0.0f, 7.5f),
	glm::vec3(-20.0f, 0.0f, 17.35f),
	glm::vec3(-5.2f, 0.0f, 11.03f),
	glm::vec3(-11.1649f, 0.0f, 6.05118f),
	glm::vec3(-17.8775f, 0.0f, 7.01931f),
};

// Projection
// perspective by default
bool perspective_view = true;
GLfloat fov = 45.0f;

// Mouse position variables
GLfloat angleChangeX = 0.0f;
GLfloat angleChangeY = 0.0f;

GLint clickOriginX = -1;
GLint clickOriginY = -1;

// Camera
GLfloat x_angle = -90.0f;
GLfloat y_angle = 0.0f;
GLfloat z_angle = 0.0f;
GLfloat mouseSens = 0.5f;

glm::vec3 cameraTarget = glm::vec3(0.0f, -0.2f, -0.9f);
glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 10.0f);

GLfloat move_speed = 0.7f;

// skybox vertices
GLfloat skyboxVertices[] = {         
	-100.0f,  100.0f, -100.0f,
	-100.0f, -100.0f, -100.0f,
	 100.0f, -100.0f, -100.0f,
	 100.0f, -100.0f, -100.0f,
	 100.0f,  100.0f, -100.0f,
	-100.0f,  100.0f, -100.0f,

	-100.0f, -100.0f,  100.0f,
	-100.0f, -100.0f, -100.0f,
	-100.0f,  100.0f, -100.0f,
	-100.0f,  100.0f, -100.0f,
	-100.0f,  100.0f,  100.0f,
	-100.0f, -100.0f,  100.0f,

	 100.0f, -100.0f, -100.0f,
	 100.0f, -100.0f,  100.0f,
	 100.0f,  100.0f,  100.0f,
	 100.0f,  100.0f,  100.0f,
	 100.0f,  100.0f, -100.0f,
	 100.0f, -100.0f, -100.0f,

	-100.0f, -100.0f,  100.0f,
	-100.0f,  100.0f,  100.0f,
	 100.0f,  100.0f,  100.0f,
	 100.0f,  100.0f,  100.0f,
	 100.0f, -100.0f,  100.0f,
	-100.0f, -100.0f,  100.0f,

	-100.0f,  100.0f, -100.0f,
	 100.0f,  100.0f, -100.0f,
	 100.0f,  100.0f,  100.0f,
	 100.0f,  100.0f,  100.0f,
	-100.0f,  100.0f,  100.0f,
	-100.0f,  100.0f, -100.0f,

	-100.0f, -100.0f, -100.0f,
	-100.0f, -100.0f,  100.0f,
	 100.0f, -100.0f, -100.0f,
	 100.0f, -100.0f, -100.0f,
	-100.0f, -100.0f,  100.0f,
	 100.0f, -100.0f,  100.0f
};

// SPRITES SECTION
// general particle struct
struct Particle {
	glm::vec3 position, velocity;
	glm::vec4 transparency;
	GLfloat   life;
	GLfloat	  scale;
	Particle()
		: position(0.0f), velocity(0.0f), transparency(1.0f), life(0.0f), scale(0.2) { }
};

// smal cubes
float particle_vertices[] = {
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,

	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f
};

unsigned int nr_fire_particles = 37;
unsigned int nr_new_fire_particles = 2;
unsigned int lastUsedFireParticle1 = 0;
unsigned int lastUsedFireParticle2 = 0;
std::vector<Particle> fire_particles1;
std::vector<Particle> fire_particles2;
GLuint particle_VAO;
GLuint particle_VBO;

unsigned int nr_blood_particles = 100;
unsigned int nr_new_blood_particles = 100;
unsigned int lastUsedBloodParticle = 0;
std::vector<Particle> blood_particles;

// Sound engine
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();

// Text section
/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint texture; // ID handle of the glyph texture
	glm::ivec2   size;      // Size of glyph
	glm::ivec2   bearing;   // Offset from baseline to left/top of glyph
	GLuint advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint char_VAO, char_VBO;
GLuint font_size = 36;
glm::vec3 text_color = glm::vec3(1.0f, 1.0f, 1.0f); // white color

glm::vec2 start_menu_coords = glm::vec2(190.0f, (GLfloat)height/2); // start message coords

glm::vec2 end_menu_coords = glm::vec2(290.0f, (GLfloat)height / 2); // end message coords
glm::vec2 your_time_coords = glm::vec2(210.0f, (GLfloat)height / 2 - 25.0f); // end time at the end, under main menu
glm::vec2 new_best_coords = glm::vec2(250.0f, (GLfloat)height / 2 - 50.0f); // new best, under your time
glm::vec2 best_time_coords = glm::vec2(260.0f, 20.0f); //best time, at the end on the bottom

glm::vec2 time_spent_coords = glm::vec2(10.0f, (GLfloat)height - 15.0f); // top left time spent
glm::vec2 ants_left_coords = glm::vec2((GLfloat)width - 150.0f, (GLfloat)height - 15.0f); // ants left, top right

GLfloat time_spent = 0.0f;
GLuint ants_left = ANT_NUMBER;
GLfloat best_time = 0.0f;
bool isStarted = false;
bool isEnded = false;
int oldTime = 0;
bool newBest = false;


#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name) {
	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */
	const aiScene* scene = aiImportFile(
		file_name, 
		aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
	); 

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
				const aiVector3D* vtan = &(mesh->mTangents[v_i]);
				modelData.mTangents.push_back(vec3(vtan->x, vtan->y, vtan->z));

				const aiVector3D* vbit = &(mesh->mBitangents[v_i]);
				modelData.mBitangents.push_back(vec3(vbit->x, vbit->y, vbit->z));
			}
		}
	}

	aiReleaseImport(scene);
	return modelData;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");

	if (fp == NULL) { 
		std::cerr << "Error reading shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
		return NULL; 
	}

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileTextureShader() {
	textureShaderProgID = glCreateProgram();
	skyboxShaderID = glCreateProgram();
	particleShaderID = glCreateProgram();
	if (textureShaderProgID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	if (skyboxShaderID == 0) {
		std::cerr << "Error creating skybox shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	if (particleShaderID == 0) {
		std::cerr << "Error creating particle shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(textureShaderProgID, "../../Lab1/textureVertexShader.glsl", GL_VERTEX_SHADER);
	AddShader(textureShaderProgID, "../../Lab1/textureFragShader.glsl", GL_FRAGMENT_SHADER);
	// Same for skybox shader
	AddShader(skyboxShaderID, "../../Lab1/skyboxVertexShader.glsl", GL_VERTEX_SHADER);
	AddShader(skyboxShaderID, "../../Lab1/skyboxFragmentShader.glsl", GL_FRAGMENT_SHADER);
	// Particle Shader
	AddShader(particleShaderID, "../../Lab1/particleVertexShader.glsl", GL_VERTEX_SHADER);
	AddShader(particleShaderID, "../../Lab1/particleFragShader.glsl", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(textureShaderProgID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(textureShaderProgID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(textureShaderProgID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking texture shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(textureShaderProgID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(textureShaderProgID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(textureShaderProgID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid texture shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Repeat for skybox shader
	glLinkProgram(skyboxShaderID);
	glGetProgramiv(skyboxShaderID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(skyboxShaderID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking skybox shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	glValidateProgram(skyboxShaderID);
	glGetProgramiv(skyboxShaderID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(skyboxShaderID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid skybox shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Particle Shader
	glLinkProgram(particleShaderID);
	glGetProgramiv(particleShaderID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(particleShaderID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking particle shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	glValidateProgram(particleShaderID);
	glGetProgramiv(particleShaderID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(particleShaderID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid particle shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// also create crowd shader
	crowdShaderID = glCreateProgram();
	if (crowdShaderID == 0) {
		std::cerr << "Error creating crowd shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// text shader
	textShaderID = glCreateProgram();
	if (textShaderID == 0) {
		std::cerr << "Error creating text shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "../../Lab1/simpleVertexShader.glsl", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "../../Lab1/simpleFragmentShader.glsl", GL_FRAGMENT_SHADER);

	// same for crowd
	AddShader(crowdShaderID, "../../Lab1/crowdVertexShader.glsl", GL_VERTEX_SHADER);
	AddShader(crowdShaderID, "../../Lab1/crowdFragShader.glsl", GL_FRAGMENT_SHADER);

	// and text
	AddShader(textShaderID, "../../Lab1/textVertexShader.glsl", GL_VERTEX_SHADER);
	AddShader(textShaderID, "../../Lab1/textFragShader.glsl", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	
	// Same for crowd shader
	glLinkProgram(crowdShaderID);
	glGetProgramiv(crowdShaderID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(crowdShaderID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// again, same for crowd
	glValidateProgram(crowdShaderID);
	glGetProgramiv(crowdShaderID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(crowdShaderID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// check text shader
	glLinkProgram(textShaderID);
	glGetProgramiv(textShaderID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(textShaderID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking text shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	glValidateProgram(textShaderID);
	glGetProgramiv(textShaderID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(textShaderID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid text shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
ModelData generateObjectBufferMesh(const char* file_name) {
	/*----------------------------------------------------------------------------
	LOAD MESH HERE AND COPY INTO BUFFERS
	----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.
	ModelData mesh_data;

	mesh_data = load_mesh(file_name);
	GLuint vp_vbo = 0;

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);
	mesh_data.vVBO = vp_vbo;

	GLuint vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);
	mesh_data.nVBO = vn_vbo;

	GLuint vao = 0;
	mesh_data.VAO = vao;

	return mesh_data;
}

void generate_characters() {
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, OCRA_FONT, 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}

	// set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, font_size);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 characters of ASCII set
	for (unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
		);
		// set texture options to cplam to endge
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// free the resources
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// generate buffers for the text
	glGenVertexArrays(1, &char_VAO);
	glGenBuffers(1, &char_VBO);
	glBindVertexArray(char_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, char_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	// we send both texture and vertex coords as one vec, and will use that in orthographic projection
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void create_texture(const char* filename, ModelData object, GLuint * global_texture, GLuint * txt_vbo) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	GLuint vt_vbo = 0;
	glGenBuffers(1, &vt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glBufferData(GL_ARRAY_BUFFER, object.mTextureCoords.size() * sizeof(vec2), &object.mTextureCoords[0], GL_STATIC_DRAW);
	*txt_vbo = vt_vbo;
	*global_texture = texture;
}

// load the best time from the file
void load_best_score() {
	std::ifstream file("../../Lab1/high_score.txt");
	std::string str;
	std::getline(file, str);
	std::istringstream in(str);
	float score;
	in >> score;
	best_time = score;
	cout << "High score loaded" << endl;
}

// save new best score
void save_score() {
	std::ofstream ofs("../../Lab1/high_score.txt", std::ofstream::trunc);
	ofs << best_time;
	ofs.close();
	cout << "High score saved" << endl;
}

void create_skybox(std::vector<std::string> filenames) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	// load the images
	int width, height, nrChannels;
	for (int i = 0; i < filenames.size(); i++) {
		unsigned char* data = stbi_load(filenames[i].c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Failed to load cubemap" << std::endl;
		}
		stbi_image_free(data);
	}
	// linear cause its far and clamp to edge, since it wont be very visible anyway
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	skybox_texture = texture;

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
}

void generateParticles(GLuint * gVAO, GLuint * gVBO) {
	// set up mesh and attribute properties
	GLuint VBO;
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	// fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(glm::vec3), &particle_vertices[0], GL_STATIC_DRAW);

	// set mesh attributes
	GLuint l1 = glGetAttribLocation(particleShaderID, "vertex");
	//GLuint l2 = glGetAttribLocation(particleShaderID, "tex_coords");
	glEnableVertexAttribArray(l1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(l1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	*gVAO = VAO;
	*gVBO = VBO;
	glBindVertexArray(0);

	// gen particles for both candles
	for (unsigned int i = 0; i < nr_fire_particles; ++i)
		fire_particles1.push_back(Particle());
	for (unsigned int i = 0; i < nr_fire_particles; ++i)
		fire_particles2.push_back(Particle());
	// and blood
	for (unsigned int i = 0; i < nr_blood_particles; ++i)
		blood_particles.push_back(Particle());
}

unsigned int FirstUnusedParticle(std::vector<Particle> particles, unsigned int * lastUsedParticle)
{
	// search from last used particle, this will usually return almost instantly
	for (unsigned int i = *lastUsedParticle; i < nr_fire_particles; ++i) {
		if (particles[i].life <= 0.0f) {
			*lastUsedParticle = i;
			return i;
		}
	}
	// otherwise, do a linear search
	for (unsigned int i = 0; i < *lastUsedParticle; ++i) {
		if (particles[i].life <= 0.0f) {
			*lastUsedParticle = i;
			return i;
		}
	}
	// override first particle if all others are alive
	*lastUsedParticle = 0;
	return 0;
}

void RespawnParticle(Particle& particle, glm::vec3 position)
{
	GLfloat rand_x = ((rand() % 10) - 5.0f) / 170.0f;
	GLfloat rand_z = ((rand() % 10) - 5.0f) / 170.0f;
	particle.position = position;
	particle.transparency = glm::vec4(1.0f, 1.0f, 0.7f, 1.0f);
	particle.life = 2.0f;
	particle.scale = 0.2f;
	particle.velocity = glm::vec3(rand_x, 0.1f, rand_z);
}
void RespawnBloodParticle(Particle& particle, glm::vec3 position)
{
	GLfloat rand_x = ((rand() % 10) - 5.0f) / 100.0f;
	GLfloat rand_z = ((rand() % 10) - 5.0f) / 100.0f;
	particle.position = position;
	particle.transparency = glm::vec4(0.6f, 0.0f, 0.0f, 1.0f);
	particle.life = 2.5f;
	particle.scale = 0.3f;
	particle.velocity = glm::vec3(rand_x, 0.0f, rand_z);
}

void render_text(std::string text, glm::vec2 coords, GLfloat scale) {
	int projection_l = glGetUniformLocation(textShaderID, "projection");
	int tcol_loc = glGetUniformLocation(textShaderID, "textColor");
	glm::mat4 P = glm::ortho(0.0f, (float)width, 0.0f, (float)height); // left, right, bottom, top, no z planes needed
	glUniformMatrix4fv(projection_l, 1, GL_FALSE, glm::value_ptr(P));
	glUniform3fv(tcol_loc, 1, glm::value_ptr(text_color));

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(char_VAO);

	// iterate through all characters in text
	std::string::const_iterator character;
	for (character = text.begin(); character != text.end(); character++)
	{
		Character ch = Characters[*character];

		float xpos = coords.x + ch.bearing.x * scale;
		// some bigger characters are drawn below the axis origin point (like p or g)
		float ypos = coords.y - (ch.size.y - ch.bearing.y) * scale;

		float wid = ch.size.x * scale;
		float high = ch.size.y * scale;
		// update VBO for each character, vertices and tex coords are generated
		float vertices[6][4] = {
			{ xpos,       ypos + high,  0.0f, 0.0f },
			{ xpos,       ypos,			0.0f, 1.0f },
			{ xpos + wid, ypos,			1.0f, 1.0f },

			{ xpos,		  ypos + high,  0.0f, 0.0f },
			{ xpos + wid, ypos,			1.0f, 1.0f },
			{ xpos + wid, ypos + high,  1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.texture);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, char_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		coords.x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void linkSkybox() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}
#pragma endregion VBO_FUNCTIONS

void linkBufferToShader(GLuint shaderID, GLuint vVBO, GLuint nVBO) {
	loc1 = glGetAttribLocation(shaderID, "vertex_position");
	loc2 = glGetAttribLocation(shaderID, "vertex_normal");

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vVBO);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, nVBO);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

}

void linkTexture(GLuint shaderID, GLuint tVBO) {
	loc3 = glGetAttribLocation(shaderID, "tex_coords");

	glEnableVertexAttribArray(loc3);
	glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void drawPart(int index) {
	glBindVertexArray(meshes[index].VAO);
	linkBufferToShader(shaderProgramID, meshes[index].vVBO, meshes[index].nVBO);
	glDrawArrays(GL_TRIANGLES, 0, meshes[index].mPointCount);
}

void drawCrowdPart(ModelData crowdPart) {
	glBindVertexArray(crowdPart.VAO);
	linkBufferToShader(crowdShaderID, crowdPart.vVBO, crowdPart.nVBO);
	glDrawArrays(GL_TRIANGLES, 0, crowdPart.mPointCount);
}

void doCrowdTransformations(glm::mat4 P, glm::mat4 V) {
	/* ================================================================
	*   In order to simplify boid behaviour and minimise computations
	*   Instead of cacluating velocity vectors of neighbours and calculating the center of the flock,
	*   our flock will be trying to move to the center, which will be somewhere ahead of them, and moving at a certain
	*   random, but consistent direction. This way we imitate boid behaviour just as well, but with a simpler implementation.
	*   In this simple scene, there should be no visual differences. Also, rest of principles is implemented as per standard.
	*  ================================================================
	*/
	// first move the flock center
	if (crowd_center.x > 25.0f && crowd_center.z > 25.0f) {
		crowd_direction = glm::vec3(-1.0f, 0.0f, -1.0f);
	}
	else if (crowd_center.x < -25.0f && crowd_center.z < -25.0f) {
		crowd_direction = glm::vec3(1.0f, 0.0f, 1.0f);
	}
	crowd_direction = glm::normalize(crowd_direction);
	crowd_center.x += crowd_direction.x * crowd_speed;
	crowd_center.z += crowd_direction.z * crowd_speed;
	glUseProgram(crowdShaderID);

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(crowdShaderID, "model");
	int view_mat_location = glGetUniformLocation(crowdShaderID, "view");
	int proj_mat_location = glGetUniformLocation(crowdShaderID, "proj");
	int camera_uniform = glGetUniformLocation(crowdShaderID, "camera_pos");
	// Material uniforms
	int ambient_loc = glGetUniformLocation(crowdShaderID, "ambient_intensity");
	int diffuse_loc = glGetUniformLocation(crowdShaderID, "diffuse_intensity");
	int specular_loc = glGetUniformLocation(crowdShaderID, "specular_intensity");
	int coeff_loc = glGetUniformLocation(crowdShaderID, "spec_coef");
	// Light uniforms
	//int ambient_col = glGetUniformLocation(crowdShaderID, "light.ambient_color");
	//int diffuse_col = glGetUniformLocation(crowdShaderID, "light.diffuse_color");
	//int specular_col = glGetUniformLocation(crowdShaderID, "light.specular_color");

	// view and projection is same for whole scene
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(V));
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(P));
	glUniform3fv(camera_uniform, 1, glm::value_ptr(cameraPos));

	// now do the crowd ants
	for (int i = 0; i < crowdAnts.size(); i++) {
		if (!crowdAnts[i].isAlive)
			continue;
		//first we update rotation variable for the animations
		crowdAnts[i].up_rot = fmodf(crowdAnts[i].up_rot + 1.0f, 360.0f);
		crowdAnts[i].mid_rot = fmodf(crowdAnts[i].mid_rot + 1.0f, 360.0f);
		crowdAnts[i].bot_rot = fmodf(crowdAnts[i].bot_rot + 1.0f, 360.0f);

		// start from initial oriantation of the ant
		glm::vec3 ant_direction = crowdAnts[i].dir;

		// going back to crowd center is lowest priority and should be smooth, 0.1 modifier
		glm::vec3 crowd_direction = glm::normalize(crowd_center - crowdAnts[i].trs) * 0.1f;
		ant_direction = glm::normalize(crowd_direction + ant_direction);

		// Predator check
		float distance = abs(glm::distance(crowdAnts[i].trs, glm::vec3(trs_x, 0.0f, trs_z)));
		if (distance < 10.0f) {
			// times 2 scale at most, since predator escape should be top most priority
			// the closer it is, the bigger priority
			glm::vec3 dir_away = glm::normalize(crowdAnts[i].trs - glm::vec3(trs_x, 0.0f, trs_z)) * (10.1f - distance) *0.15f;
			ant_direction = glm::normalize(dir_away + ant_direction);
		}

		// Neighbour collision check
		for (int j = 0; j < crowdAnts.size(); j++) {
			float distance = abs(glm::distance(crowdAnts[i].trs, crowdAnts[j].trs));
			if (i != j && distance < 7.5f) {
				// times 1.25 scale at most, since interboid collision is medium priority
				// again, closer = bigger priority
				glm::vec3 dir_away = crowdAnts[i].trs - crowdAnts[j].trs;
				// edge case, we cant normalize 0 vectors
				if (dir_away.x == 0.0 && dir_away.z == 0.0) {
					float x_rand = float(rand() % 10 + 1) / 10.0f;
					float z_rand = float(rand() % 10 + 1) / 10.0f;
					// scatter the crowd
					dir_away.x = x_rand;
					dir_away.z = z_rand;
				}
				dir_away = glm::normalize(dir_away) * (7.6f - distance)* 0.13f;
				ant_direction = glm::normalize(dir_away + ant_direction);
			}
		}

		// Rock collision avoidance
		for (int j = 0; j < collision_points.size(); j++) {
			float distance = abs(glm::distance(crowdAnts[i].trs, collision_points[j]));
			if (distance < 3.9f) {
				glm::vec3 dir_away = crowdAnts[i].trs - collision_points[j];
				dir_away = glm::normalize(dir_away) * (4.0f - distance) * 0.20f;
				ant_direction = glm::normalize(dir_away + ant_direction);
			}
		}
		// Border collision check
		glm::vec3 dir_away;
		if (abs(crowdAnts[i].trs.x - x_border) < 3.0f) {
			if ((crowdAnts[i].trs.x - x_border) > 0.0f) {
				dir_away = glm::vec3(1.0f, 0.0f, 0.0f) * 0.25f;
			}
			else {
				dir_away = glm::vec3(-1.0f, 0.0f, 0.0f) * 0.25f;
			}
			ant_direction = glm::normalize(dir_away + ant_direction);
		}
		if (abs(crowdAnts[i].trs.z - z_border) < 2.0f) {
			if ((crowdAnts[i].trs.z - z_border) > 0.0f) {
				dir_away = glm::vec3(0.0f, 0.0f, 1.0f) * 0.25f;
			}
			else {
				dir_away = glm::vec3(0.0f, 0.0f, -1.0f) * 0.25f;
			}
			ant_direction = glm::normalize(dir_away + ant_direction);
		}

		// Border safety translation
		if (crowdAnts[i].trs.x > x_border) {
			crowdAnts[i].trs.x = x_border - 0.1f;
		}
		else if (crowdAnts[i].trs.x < -x_border) {
			crowdAnts[i].trs.x = -x_border + 0.1f;
		}
		if (crowdAnts[i].trs.z > z_border) {
			crowdAnts[i].trs.z = z_border - 0.1f;
		}
		else if (crowdAnts[i].trs.z < -z_border) {
			crowdAnts[i].trs.z = -z_border + 0.1f;
		}

		// finally calculate rotation angle and translations
		crowdAnts[i].rotate_y = -(glm::degrees(atan2(ant_direction.z, ant_direction.x)));
		crowdAnts[i].trs.x += ant_direction.x * crowdAnts[i].speed;
		crowdAnts[i].trs.z += ant_direction.z * crowdAnts[i].speed;
		crowdAnts[i].dir = ant_direction;

		glm::mat4 model(1.0f);
		glm::mat4 T(1.0f);
		glm::mat4 R(1.0f);
		glm::mat4 S(1.0f);

		R = glm::rotate(R, glm::radians(crowdAnts[i].rotate_y), glm::vec3(0, 1, 0));
		S = glm::scale(S, glm::vec3(crowdAnts[i].scale_x, crowdAnts[i].scale_y, crowdAnts[i].scale_z));
		T = glm::translate(T, crowdAnts[i].trs);

		model *= T * R * S;
		glUniform3fv(ambient_loc, 1, glm::value_ptr(crowdAnts[i].mat_properties.ambient));
		glUniform3fv(diffuse_loc, 1, glm::value_ptr(crowdAnts[i].mat_properties.diffuse));
		glUniform3fv(specular_loc, 1, glm::value_ptr(crowdAnts[i].mat_properties.specular));
		glUniform1f(coeff_loc, crowdAnts[i].mat_properties.s_coefficient);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model));
		drawCrowdPart(crowdAnts[i].body);

		glm::mat4 legR(1.0f);
		legR = glm::translate(legR, glm::vec3(x_off, 0, 0));
		legR = glm::rotate(legR, glm::radians(cos(glm::radians(crowdAnts[i].up_rot) * crowdAnts[i].leg_speed) * 15.0f), glm::vec3(0, 1, 0));
		legR = glm::translate(legR, glm::vec3(-x_off, 0, 0));
		legR = model * legR;
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(legR));
		drawCrowdPart(crowdAnts[i].up_left);

		glm::mat4 identity(1.0f);
		legR = identity;
		legR = glm::translate(legR, glm::vec3(x_off, 0, 0));
		legR = glm::rotate(legR, glm::radians(sin(glm::radians(crowdAnts[i].up_rot) * crowdAnts[i].leg_speed) * 15.0f), glm::vec3(0, 1, 0));
		legR = glm::translate(legR, glm::vec3(-x_off, 0, 0));
		legR = model * legR;
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(legR));
		drawCrowdPart(crowdAnts[i].up_right);

		legR = identity;
		legR = glm::translate(legR, glm::vec3(x_off / 2, 0, 0));
		legR = glm::rotate(legR, glm::radians(sin(glm::radians(crowdAnts[i].mid_rot) * crowdAnts[i].leg_speed) * 15.0f), glm::vec3(0, 1, 0));
		legR = glm::translate(legR, glm::vec3(-x_off / 2, 0, 0));
		legR = model * legR;
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(legR));
		drawCrowdPart(crowdAnts[i].mid_left);

		legR = identity;
		legR = glm::translate(legR, glm::vec3(x_off / 2, 0, 0));
		legR = glm::rotate(legR, glm::radians(cos(glm::radians(-(crowdAnts[i].mid_rot)) * crowdAnts[i].leg_speed) * 15.0f), glm::vec3(0, 1, 0));
		legR = glm::translate(legR, glm::vec3(-x_off / 2, 0, 0));
		legR = model * legR;
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(legR));
		drawCrowdPart(crowdAnts[i].mid_right);

		legR = identity;
		legR = glm::translate(legR, glm::vec3(x_off / 2, 0, 0));
		legR = glm::rotate(legR, glm::radians(cos(glm::radians(crowdAnts[i].bot_rot) * crowdAnts[i].leg_speed) * 15.0f), glm::vec3(0, 1, 0));
		legR = glm::translate(legR, glm::vec3(-x_off / 2, 0, 0));
		legR = model * legR;
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(legR));
		drawCrowdPart(crowdAnts[i].bot_left);

		legR = identity;
		legR = glm::translate(legR, glm::vec3(x_off / 2, 0, 0));
		legR = glm::rotate(legR, glm::radians(sin(glm::radians(crowdAnts[i].bot_rot) * crowdAnts[i].leg_speed) * 15.0f), glm::vec3(0, 1, 0));
		legR = glm::translate(legR, glm::vec3(-x_off / 2, 0, 0));
		legR = model * legR;
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(legR));
		drawCrowdPart(crowdAnts[i].bot_right);
	}
}

void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_BLEND);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
	int camera_uniform = glGetUniformLocation(shaderProgramID, "camera_pos");
	// Material uniforms
	int ambient_loc = glGetUniformLocation(shaderProgramID, "ambient_intensity");
	int diffuse_loc = glGetUniformLocation(shaderProgramID, "diffuse_intensity");
	int specular_loc = glGetUniformLocation(shaderProgramID, "specular_intensity");
	int coeff_loc = glGetUniformLocation(shaderProgramID, "spec_coef");
	// Light uniforms
	//int ambient_col = glGetUniformLocation(crowdShaderID, "light.ambient_color");
	//int diffuse_col = glGetUniformLocation(crowdShaderID, "light.diffuse_color");
	//int specular_col = glGetUniformLocation(crowdShaderID, "light.specular_color");

	// Border collision wall stop
	if (trs_x > x_border) {
		trs_x = x_border;
	}
	else if (trs_x < -x_border) {
		trs_x = -x_border;
	}
	if (trs_z > z_border) {
		trs_z = z_border;
	}
	else if (trs_z < -z_border) {
		trs_z = -z_border;
	}
	// rock collision wall stop
	// rock 1
	if (trs_x > 6.8f && trs_x < 18.0f && trs_z < -12.2f && trs_z > -19.2f) {
		trs_x -= 0.11f * cos(glm::radians(rotate_y));
		trs_z += 0.11f * sin(glm::radians(rotate_y));
	}
	// rock 2
	if (trs_x > -20.0f && trs_x < -5.2f && trs_z > 5.5f && trs_z < 14.9f) {
		trs_x -= 0.11f * cos(glm::radians(rotate_y));
		trs_z += 0.11f * sin(glm::radians(rotate_y));
	}

	// small ant collision detection
	for (int j = 0; j < crowdAnts.size(); j++) {
		GLfloat distance = abs(glm::distance(glm::vec3(trs_x, 0.0f, trs_z), crowdAnts[j].trs));
		if (distance < body_lengths && crowdAnts[j].isAlive) {
			// kill the ant
			crowdAnts[j].isAlive = false;
			ants_left--;
			// spawn blood particles
			for (unsigned int i = 0; i < nr_new_blood_particles; ++i)
			{
				int unusedParticle = FirstUnusedParticle(blood_particles, &lastUsedBloodParticle);
				RespawnBloodParticle(blood_particles[unusedParticle], crowdAnts[j].trs);
			}
			SoundEngine->play2D(SPLAT_SOUND, false);
		}
	}

	// Root of the Hierarchy
	// ORDER: SCALE -> ROTATE -> TRANSLATE
	// When combining, do it in reverse order!
	// Model
	glm::mat4 model(1.0f);
	glm::mat4 T(1.0f);
	glm::mat4 R(1.0f);
	glm::mat4 S(1.0f);

	R = glm::rotate(R, glm::radians(rotate_x), glm::vec3(1, 0, 0));
	R = glm::rotate(R, glm::radians(rotate_y), glm::vec3(0, 1, 0));
	R = glm::rotate(R, glm::radians(rotate_z), glm::vec3(0, 0, 1));

	S = glm::scale(S, glm::vec3(scale_x, scale_y, scale_z));
	T = glm::translate(T, glm::vec3(trs_x, trs_y, trs_z));

	model *= T * R * S;

	// View
	glm::mat4 V(1.0f);
	V = glm::lookAt(cameraPos, cameraPos + cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f)); //position, target, up direction

	// Projection
	glm::mat4 P(1.0f);
	// fov, aspect ratio, near plane, far plane (planes mean when does the plane actually start and end, relatively to camera pos) 
	P = glm::perspective(glm::radians(fov), (float)width/(float)height, 1.0f, 250.0f ); 

	mat4 persp_proj = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(P));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(V));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(camera_uniform, 1, glm::value_ptr(cameraPos));
	glUniform3fv(ambient_loc, 1, glm::value_ptr(ant_properties.ambient));
	glUniform3fv(diffuse_loc, 1, glm::value_ptr(ant_properties.diffuse));
	glUniform3fv(specular_loc, 1, glm::value_ptr(ant_properties.specular));
	glUniform1f(coeff_loc, ant_properties.s_coefficient);

	// MAIN ANT BODY DRAW
	// We know the order of parts in our vector, so we go through that manually
	int antPartIndex = 0;
	//cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << endl;
	// Body draw
	drawPart(antPartIndex++);

	// Set up the up legs child matrices
	glm::mat4 upLeftLegModel(1.0f);
	glm::mat4 upRightLegModel(1.0f);
	glm::mat4 botLeftLegModel(1.0f);
	glm::mat4 botRightLegModel(1.0f);
	glm::mat4 midLeftLegModel(1.0f);
	glm::mat4 midRightLegModel(1.0f);

	glm::mat4 identity(1.0f);
	glm::mat4 legR(1.0f);
	legR = glm::translate(legR, glm::vec3(x_off, 0, 0));
	legR = glm::rotate(legR, glm::radians(cos(glm::radians(up_rota)*leg_speed) * 15.0f), glm::vec3(0, 1, 0));
	legR = glm::translate(legR, glm::vec3(-x_off, 0, 0));
	upLeftLegModel = upLeftLegModel * legR;
	// Connect to parent
	upLeftLegModel = model * upLeftLegModel;
	// Update and draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(upLeftLegModel));
	drawPart(antPartIndex++);

	legR = identity;
	legR = glm::translate(legR, glm::vec3(x_off, 0, 0));
	legR = glm::rotate(legR, glm::radians(sin(glm::radians(up_rota) * leg_speed) * 15.0f), glm::vec3(0, 1, 0));
	legR = glm::translate(legR, glm::vec3(-x_off, 0, 0));
	upRightLegModel = upRightLegModel * legR;
	upRightLegModel = model * upRightLegModel;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(upRightLegModel));
	drawPart(antPartIndex++);

	legR = identity;
	legR = glm::translate(legR, glm::vec3(x_off / 2, 0, 0));
	legR = glm::rotate(legR, glm::radians(sin(glm::radians(mid_rota) * leg_speed) * 15.0f), glm::vec3(0, 1, 0));
	legR = glm::translate(legR, glm::vec3(-x_off / 2, 0, 0));
	midLeftLegModel = midLeftLegModel * legR;
	midLeftLegModel = model * midLeftLegModel;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(midLeftLegModel));
	drawPart(antPartIndex++);

	legR = identity;
	legR = glm::translate(legR, glm::vec3(x_off / 2, 0, 0));
	legR = glm::rotate(legR, glm::radians(cos(glm::radians(-mid_rota) * leg_speed) * 15.0f), glm::vec3(0, 1, 0));
	legR = glm::translate(legR, glm::vec3(-x_off / 2, 0, 0));
	midRightLegModel = midRightLegModel * legR;
	midRightLegModel = model * midRightLegModel;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(midRightLegModel));
	drawPart(antPartIndex++);

	legR = identity;
	legR = glm::translate(legR, glm::vec3(x_off/2, 0, 0));
	legR = glm::rotate(legR, glm::radians(cos(glm::radians(bot_rota) * leg_speed) * 15.0f), glm::vec3(0, 1, 0));
	legR = glm::translate(legR, glm::vec3(-x_off/2, 0, 0));
	botLeftLegModel = botLeftLegModel * legR;
	botLeftLegModel = model * botLeftLegModel;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(botLeftLegModel));
	drawPart(antPartIndex++);

	legR = identity;
	legR = glm::translate(legR, glm::vec3(x_off/2, 0, 0));
	legR = glm::rotate(legR, glm::radians(sin(glm::radians(bot_rota) * leg_speed) * 15.0f), glm::vec3(0, 1, 0));
	legR = glm::translate(legR, glm::vec3(-x_off/2, 0, 0));
	botRightLegModel = botRightLegModel * legR;
	botRightLegModel = model * botRightLegModel;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(botRightLegModel));
	drawPart(antPartIndex++);

	// Draw candles
	glm::mat4 candleModel(1.0f);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(candleModel));
	drawPart(antPartIndex++);

	// textured models region
	glUseProgram(textureShaderProgID);

	//Declare your uniform variables that will be used in your shader
	int texture_model_location = glGetUniformLocation(textureShaderProgID, "model");
	int view_plane_location = glGetUniformLocation(textureShaderProgID, "view");
	int proj_plane_location = glGetUniformLocation(textureShaderProgID, "proj");
	GLuint normal_bool = glGetUniformLocation(textureShaderProgID, "hasNormals");
	GLuint normal_bool_v = glGetUniformLocation(textureShaderProgID, "hasNormalsV");
	glUniform1i(normal_bool, 0);

	// Plane should always be last one the list
	glm::mat4 planeModel(1.0f);
	glBindVertexArray(meshes.back().VAO);
	linkBufferToShader(textureShaderProgID, meshes.back().vVBO, meshes.back().nVBO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass_texture);
	linkTexture(textureShaderProgID, gr_planeVBO);
	glUniformMatrix4fv(proj_plane_location, 1, GL_FALSE, glm::value_ptr(P));
	glUniformMatrix4fv(view_plane_location, 1, GL_FALSE, glm::value_ptr(V));
	glUniformMatrix4fv(texture_model_location, 1, GL_FALSE, glm::value_ptr(planeModel));
	glDrawArrays(GL_TRIANGLES, 0, meshes.back().mPointCount);

	// Rendering rocks
	glm::mat4 rockModel(1.0f);
	GLuint normal_sampler = glGetUniformLocation(textureShaderProgID, "normal_sampler");
	glUniform1i(normal_sampler, 1);
	glUniform1i(normal_bool, 1);
	glUniform1i(normal_bool_v, 1);
	glBindVertexArray(meshes[antPartIndex].VAO);
	linkBufferToShader(textureShaderProgID, meshes[antPartIndex].vVBO, meshes[antPartIndex].nVBO);
	/*// tangents and bitangets 
	GLuint loc = glGetAttribLocation(textureShaderProgID, "tangents");
	GLuint vtan_vbo = 0;
	glGenBuffers(1, &vtan_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vtan_vbo);
	glBufferData(GL_ARRAY_BUFFER, meshes[antPartIndex].mPointCount * sizeof(vec3), &meshes[antPartIndex].mTangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	GLuint b_loc = glGetAttribLocation(textureShaderProgID, "bitangents");
	GLuint vbtan_vbo = 0;
	glGenBuffers(1, &vbtan_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbtan_vbo);
	glBufferData(GL_ARRAY_BUFFER, meshes[antPartIndex].mPointCount * sizeof(vec3), &meshes[antPartIndex].mBitangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(b_loc);
	glVertexAttribPointer(b_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	*/

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rock_color);
	linkTexture(textureShaderProgID, rock_cVBO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rock_normals);
	linkTexture(textureShaderProgID, rock_nVBO);
	glUniformMatrix4fv(texture_model_location, 1, GL_FALSE, glm::value_ptr(rockModel));
	glDrawArrays(GL_TRIANGLES, 0, meshes[antPartIndex].mPointCount);


	// Rendering crowd
	if (isStarted && !isEnded) {
		doCrowdTransformations(P, V);
	}

	// Rendering skybox
	//glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glUseProgram(skyboxShaderID);
	glDepthMask(GL_FALSE);
	int view_loc = glGetUniformLocation(skyboxShaderID, "view");
	int proj_loc = glGetUniformLocation(skyboxShaderID, "projection");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(V));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(P));
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	linkSkybox();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	
	// Rendering Particles
	glUseProgram(particleShaderID);
	int view_l = glGetUniformLocation(particleShaderID, "view");
	int proj_l = glGetUniformLocation(particleShaderID, "projection");
	int pos_loc = glGetUniformLocation(particleShaderID, "position");
	int color_loc = glGetUniformLocation(particleShaderID, "color");
	int scale_loc = glGetUniformLocation(particleShaderID, "scale");
	glUniformMatrix4fv(view_l, 1, GL_FALSE, glm::value_ptr(V));
	glUniformMatrix4fv(proj_l, 1, GL_FALSE, glm::value_ptr(P));
	// add new fire particles
	for (unsigned int i = 0; i < nr_new_fire_particles; ++i)
	{
		int unusedParticle = FirstUnusedParticle(fire_particles1, &lastUsedFireParticle1);
		RespawnParticle(fire_particles1[unusedParticle], glm::vec3(-15.30f, 7.7f, 10.60f));

		unusedParticle = FirstUnusedParticle(fire_particles2, &lastUsedFireParticle2);
		RespawnParticle(fire_particles2[unusedParticle], glm::vec3(17.35f, 4.7f, -15.95f));
	}
	// update all fire particles
	for (unsigned int i = 0; i < nr_fire_particles; ++i)
	{
		Particle& p = fire_particles1[i];
		Particle& p2 = fire_particles2[i];
		p.life -= 0.1f; // reduce life
		p2.life -= 0.1f; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.position += p.velocity;
			p.transparency.a -= 0.05f;
			p.scale -= 0.01f;
		}
		if (p2.life > 0.0f)
		{	// particle is alive, thus update
			p2.position += p2.velocity;
			p2.transparency.a -= 0.05f;
			p2.scale -= 0.01f;
		}
	}
	// update all blood particles
	for (unsigned int i = 0; i < nr_blood_particles; ++i)
	{
		Particle& p = blood_particles[i];
		p.life -= 0.1f; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.velocity += glm::vec3(0.0f, -0.004f, 0.0f);
			p.position += p.velocity;
			p.transparency.a -= 0.03f;
			p.scale -= 0.01f;
		}
	}
	// draw them
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (Particle particle : fire_particles1)
	{
		if (particle.life > 0.0f)
		{
			glUniform3fv(pos_loc, 1, glm::value_ptr(particle.position));
			glUniform4fv(color_loc, 1, glm::value_ptr(particle.transparency));
			glUniform1f(scale_loc, particle.scale);
			glBindVertexArray(particle_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
	for (Particle particle : fire_particles2)
	{
		if (particle.life > 0.0f)
		{
			glUniform3fv(pos_loc, 1, glm::value_ptr(particle.position));
			glUniform4fv(color_loc, 1, glm::value_ptr(particle.transparency));
			glUniform1f(scale_loc, particle.scale);
			glBindVertexArray(particle_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
	for (Particle particle : blood_particles)
	{
		if (particle.life > 0.0f)
		{
			glUniform3fv(pos_loc, 1, glm::value_ptr(particle.position));
			glUniform4fv(color_loc, 1, glm::value_ptr(particle.transparency));
			glUniform1f(scale_loc, particle.scale);
			glBindVertexArray(particle_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Text rendering
	glUseProgram(textShaderID);

	if (!isStarted) {
		render_text("Press ENTER to start", start_menu_coords, 1.0f);
	} else if (isEnded) {
		if (time_spent < best_time) {
			best_time = time_spent;
			save_score();
			newBest = true;
		}
		std::string t = std::to_string(time_spent);
		std::string b = std::to_string(best_time);
		render_text("You won! ", end_menu_coords, 1.0f);
		render_text("Your final time was: "+t, your_time_coords, 0.7f);
		render_text("Best time: "+b, best_time_coords, 0.7f);
		if (newBest) {
			render_text("You got new best time!", new_best_coords, 0.7f);
		}
	}
	else {
		std::string s = std::to_string(ants_left);
		render_text("Ants left: "+s, ants_left_coords, 0.5f);

		int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		int deltaTime = timeSinceStart - oldTime;
		oldTime = timeSinceStart;
		time_spent += (GLfloat)deltaTime / 1000.0f;
		std::string t = std::to_string(time_spent);
		render_text("Your time: "+t, time_spent_coords, 0.5f);
	}

	if (ants_left == 0) {
		isEnded = true;
	}
			
	glutSwapBuffers();
}

void updateScene() {

	// Draw the next frame
	glutPostRedisplay();
}

void loadUserAntMesh() {
	meshes.push_back(generateObjectBufferMesh(ANT_BODY));
	meshes.push_back(generateObjectBufferMesh(ANT_UP_LEFT));
	meshes.push_back(generateObjectBufferMesh(ANT_UP_RIGHT));
	meshes.push_back(generateObjectBufferMesh(ANT_MID_LEFT));
	meshes.push_back(generateObjectBufferMesh(ANT_MID_RIGHT));
	meshes.push_back(generateObjectBufferMesh(ANT_BOT_LEFT));
	meshes.push_back(generateObjectBufferMesh(ANT_BOT_RIGHT));
}

void loadCrowdAntMeshes() {
	for (int i = 0; i < ANT_NUMBER; i++) {
		Ant newAnt = Ant();
		newAnt.body = generateObjectBufferMesh(ANT_BODY);
		newAnt.up_left = generateObjectBufferMesh(ANT_UP_LEFT);
		newAnt.up_right = generateObjectBufferMesh(ANT_UP_RIGHT);
		newAnt.mid_left = generateObjectBufferMesh(ANT_MID_LEFT);
		newAnt.mid_right = generateObjectBufferMesh(ANT_MID_RIGHT);
		newAnt.bot_left = generateObjectBufferMesh(ANT_BOT_LEFT);
		newAnt.bot_right = generateObjectBufferMesh(ANT_BOT_RIGHT);
		newAnt.mat_properties = crowdMats[fmod(i, crowdMats.size())];
		crowdAnts.push_back(newAnt);
	}
}

// Materials of crowd ants
void create_crowd_materials() {
	// Obsidian
	Material newMat = Material();
	newMat.ambient = glm::vec3(0.05375f, 0.05f, 0.06625f);
	newMat.diffuse = glm::vec3(0.18275f, 0.17f, 0.22525f);
	newMat.specular = glm::vec3(0.332741f, 0.328634f, 0.346435f);
	newMat.s_coefficient = 38.4f;
	crowdMats.push_back(newMat);

	// Black plastic
	newMat = Material();
	newMat.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	newMat.diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
	newMat.specular = glm::vec3(0.50f, 0.50f, 0.50f);
	newMat.s_coefficient = 32.0f;
	crowdMats.push_back(newMat);

	// Black rubber
	newMat = Material();
	newMat.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
	newMat.diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
	newMat.specular = glm::vec3(0.4f, 0.4f, 0.4f);
	newMat.s_coefficient = 10.0f;
	crowdMats.push_back(newMat);

	// Gold
	newMat = Material();
	newMat.ambient = glm::vec3(0.24725f, 0.1995f, 0.0745f);
	newMat.diffuse = glm::vec3(0.75164f, 0.60648f, 0.22648f);
	newMat.specular = glm::vec3(0.628281f, 0.555802f, 0.366065f);
	newMat.s_coefficient = 51.2f;
	crowdMats.push_back(newMat);

	// Pearl
	newMat = Material();
	newMat.ambient = glm::vec3(0.25f, 0.20725f, 0.20725f);
	newMat.diffuse = glm::vec3(1.0f, 0.829f, 0.829f);
	newMat.specular = glm::vec3(0.296648f, 0.296648f, 0.296648f);
	newMat.s_coefficient = 11.264f;
	crowdMats.push_back(newMat);
}

void init()
{
	// Set up the shaders
	CompileShaders();
	CompileTextureShader();

	// Create materials
	create_crowd_materials();

	// load meshes and initialise buffers
	loadUserAntMesh();
	// Ruby material
	ant_properties.ambient = glm::vec3(0.1745f, 0.01175f, 0.01175f);
	ant_properties.diffuse = glm::vec3(0.61424f, 0.04136f, 0.04136f);
	ant_properties.specular = glm::vec3(0.727811, 0.626959, 0.626959);
	ant_properties.s_coefficient = 76.8f;
	loadCrowdAntMeshes();

	ModelData candles = generateObjectBufferMesh(CANDLES);
	meshes.push_back(candles);

	ModelData rocks = generateObjectBufferMesh(ROCKS);
	create_texture(ROCK_NORMAL_TEXTURE, rocks, &rock_normals, &rock_nVBO);
	create_texture(ROCK_COLOR_TEXTURE, rocks, &rock_color, &rock_cVBO);
	meshes.push_back(rocks);

	ModelData plane = generateObjectBufferMesh(PLANE);
	create_texture(GRASS_TEXTURE, plane, &grass_texture, &gr_planeVBO);
	meshes.push_back(plane);

	create_skybox(filenames);

	generateParticles(&particle_VAO, &particle_VBO);

	generate_characters();

	load_best_score();
}

// Special keypresses function
void specialKeyPress(int key, int x, int y) {
	int mod = glutGetModifiers();
	glm::vec3 forwardVector = cameraTarget;
	glm::normalize(forwardVector);
	if (!isStarted || isEnded) {
		return;
	}
	// Cam right
	if (key == GLUT_KEY_RIGHT) {
		glm::vec3 rightVector = glm::cross(forwardVector, glm::vec3(0.0f, 1.0f, 0.0f));
		cameraPos += rightVector * move_speed;
	}
	// Cam left
	else if (key == GLUT_KEY_LEFT) {
		glm::vec3 leftVector = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forwardVector);
		cameraPos += leftVector * move_speed;
	}
	else if (key == GLUT_KEY_UP && mod == GLUT_ACTIVE_SHIFT) {

	}
	else if (key == GLUT_KEY_DOWN && mod == GLUT_ACTIVE_SHIFT) {

	}
	// Cam forward
	else if (key == GLUT_KEY_UP) {
		cameraPos += forwardVector * move_speed;
	}
	// Cam backwards
	else if (key == GLUT_KEY_DOWN) {
		cameraPos -= forwardVector * move_speed;
	} 

	// Redraw the frame
	glutPostRedisplay();

}

// Function for the keypress
void keypress(unsigned char key, int x, int y) {

	if ((!isStarted || isEnded) && key != 13) {
		return;
	}
	// scaling up all
	if (key == '+') {
		scale_x += 0.1f;
		scale_y += 0.1f;
		scale_z += 0.1f;
	}
	// scaling down all
	if (key == '-') {
		scale_x -= 0.1f;
		scale_y -= 0.1f;
		scale_z -= 0.1f;
	}
	if (key == 'X' ) {
	}
	if (key == 'x') {
	}
	if (key == 'y') {
	}
	if (key == 'Y') {
	}
	if (key == 'z') {
	}
	// scaling down z
	if (key == 'Z') {
	}
	if (key == 's') {
		// x and z will depend on direction, using simple trigonometry
		trs_x -= 0.14f * cos(glm::radians(rotate_y));
		trs_z += 0.14f * sin(glm::radians(rotate_y));
		up_rota -= 1.0f;
		up_rota = fmodf(up_rota, 360.0f);
		mid_rota -= 1.0f;
		mid_rota = fmodf(mid_rota, 360.0f);
		bot_rota -= 1.0f;
		bot_rota = fmodf(bot_rota, 360.0f);
	}
	if (key == 'w') {
		// x and z will depend on direction, using simple trigonometry
		trs_x += 0.14f * cos(glm::radians(rotate_y));
		trs_z -= 0.14f * sin(glm::radians(rotate_y));
		up_rota += 1.0f;
		up_rota = fmodf(up_rota, 360.0f);
		mid_rota += 1.0f;
		mid_rota = fmodf(mid_rota, 360.0f);
		bot_rota += 1.0f;
		bot_rota = fmodf(bot_rota, 360.0f);
	}
	if (key == 'a') {
		// Rotate the model slowly around the y axis at 1 degrees per frame
		rotate_y += 1.0f;
		rotate_y = fmodf(rotate_y, 360.0f);
		up_rota -= 1.0f;
		up_rota = fmodf(up_rota, 360.0f);
		mid_rota += 1.0f;
		mid_rota = fmodf(mid_rota, 360.0f);
		bot_rota -= 1.0f;
		bot_rota = fmodf(bot_rota, 360.0f);
	}
	if (key == 'd') {
		// Rotate the model slowly around the y axis at 1 degrees per frame
		rotate_y -= 1.0f;
		rotate_y = fmodf(rotate_y, 360.0f);
		up_rota += 1.0f;
		up_rota = fmodf(up_rota, 360.0f);
		mid_rota -= 1.0f;
		mid_rota = fmodf(mid_rota, 360.0f);
		bot_rota += 1.0f;
		bot_rota = fmodf(bot_rota, 360.0f);
	}
	if (key == 'q') {

	}
	if (key == 'e') {
		
	}
	if (key == 'p') {
	}
	// enter key
	if (key == 13) {
		if (!isStarted) {
			oldTime = glutGet(GLUT_ELAPSED_TIME);
		}
		isStarted = true;
	}

	// Draw the next frame
	glutPostRedisplay();
}

// Mouse buttons processing
void mouseButton(int button, int state, int x, int y) {
	// on left press
	if (button == GLUT_LEFT_BUTTON) {
		// when the button is released reset the click origin
		if (state == GLUT_UP) {
			clickOriginX = -1;
			clickOriginY = -1;
		}
		// if that was the press then save the origin
		else {// state = GLUT_DOWN
			clickOriginX = x;
			clickOriginY = y;
		}
	}
}

// Mouse movement while pressed processing
void mouseMove(int x, int y) {
	if (!isStarted || isEnded) {
		return;
	}
	// glut origin is top left
	if (clickOriginX >= 0) {
		angleChangeX = (x - clickOriginX) * mouseSens;
		angleChangeY = (clickOriginY - y) * mouseSens;

		// update camera's target angles
		x_angle += angleChangeX;
		y_angle += angleChangeY;

		clickOriginX = x;
		clickOriginY = y;

		if (y_angle > 89.0f)
			y_angle = 89.0f;
		if (y_angle < -89.0f)
			y_angle = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(x_angle)) * cos(glm::radians(y_angle));
		direction.y = sin(glm::radians(y_angle));
		direction.z = sin(glm::radians(x_angle)) * cos(glm::radians(y_angle));
		cameraTarget = glm::normalize(direction);
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Spooky Scene");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(specialKeyPress);

	// Mouse
	// on press
	glutMouseFunc(mouseButton);
	// while mouse is pressed
	glutMotionFunc(mouseMove);
	// while mouse not pressed
	//glutPassiveMotionFunc();

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
