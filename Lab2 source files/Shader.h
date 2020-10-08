#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>

class Shader
{
private :
	GLuint programID;

	std::string loadShaderSource(const char* filename)
	{
		std::string line = "";
		std::string fullFile = "";


		std::ifstream myfile;
		myfile.open(filename);

		if (myfile.is_open())
		{
			//Read the whole file line by line
			while (std::getline(myfile, line))
				fullFile += line + "\n";
		}
		else
		{
			std::cout << "Error opening/reading shader file " << filename << "\n";
			exit(1);
		}

		myfile.close();

		return fullFile;
	}

	GLuint loadShader(GLenum type, const char* filename)
	{
		GLuint shader = glCreateShader(type);
		std::string shaderSource = this->loadShaderSource(filename);
		// convert to char array
		const GLchar* charSource = shaderSource.c_str();

		glShaderSource(shader, 1, &charSource, NULL);
		glCompileShader(shader);

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[1024];
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			fprintf(stderr, "Error compiling shader type %d: '%s'\n", type, infoLog);
			exit(1);
		}

		return shader;
	}

	void linkToProgram(GLuint vertexShader, GLuint fragmentShader)
	{
		this->programID = glCreateProgram();

		glAttachShader(this->programID, vertexShader);
		glAttachShader(this->programID, fragmentShader);
		glLinkProgram(this->programID);

		GLint success;
		GLchar errorLog[1024] = { 0 };
		glGetProgramiv(this->programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->programID, sizeof(errorLog), NULL, errorLog);
			fprintf(stderr, "Invalid shader program: '%s'\n", errorLog);
			exit(1);
		}

		// validate the program
		glValidateProgram(this->programID);
		glGetProgramiv(this -> programID, GL_VALIDATE_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->programID, sizeof(errorLog), NULL, errorLog);
			fprintf(stderr, "Invalid shader program: '%s'\n", errorLog);
			exit(1);
		}
		glUseProgram(this->programID);
	}

public:

	//Constructor
	Shader(const char* vertexFile, const char* fragmentFile)
	{
		GLuint vertexShader;
		GLuint fragmentShader;

		vertexShader = loadShader(GL_VERTEX_SHADER, vertexFile);
		fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentFile);

		this->linkToProgram(vertexShader, fragmentShader);

		//Flag for deletion, for when program will be finishing or deleted
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	~Shader()
	{
		glDeleteProgram(this->programID);
	}


	GLuint getProgramID()
	{
		return this->programID;
	}

	void use()
	{
		glUseProgram(this->programID);
	}

	void unuse()
	{
		glUseProgram(0);
	}

	// Uniform functions for updating uniform variables
	void set1i(GLint value, const GLchar* name)
	{
		this->use();

		glUniform1i(glGetUniformLocation(this->programID, name), value);

		this->unuse();
	}

	void set1f(GLfloat value, const GLchar* name)
	{
		this->use();

		glUniform1f(glGetUniformLocation(this->programID, name), value);

		this->unuse();
	}

	void setVec2f(glm::fvec2 value, const GLchar* name)
	{
		this->use();

		glUniform2fv(glGetUniformLocation(this->programID, name), 1, glm::value_ptr(value));

		this->unuse();
	}

	void setVec3f(glm::fvec3 value, const GLchar* name)
	{
		this->use();

		glUniform3fv(glGetUniformLocation(this->programID, name), 1, glm::value_ptr(value));

		this->unuse();
	}

	void setVec4f(glm::fvec4 value, const GLchar* name)
	{
		this->use();

		glUniform4fv(glGetUniformLocation(this->programID, name), 1, glm::value_ptr(value));

		this->unuse();
	}

	void setMat3fv(glm::mat3 value, const GLchar* name, GLboolean transpose = GL_FALSE)
	{
		this->use();

		glUniformMatrix3fv(glGetUniformLocation(this->programID, name), 1, transpose, glm::value_ptr(value));

		this->unuse();
	}

	void setMat4fv(glm::mat4 value, const GLchar* name, GLboolean transpose = GL_FALSE)
	{
		this->use();

		glUniformMatrix4fv(glGetUniformLocation(this->programID, name), 1, transpose, glm::value_ptr(value));

		this->unuse();
	}


};
