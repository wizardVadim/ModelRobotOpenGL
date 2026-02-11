#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class Shader {
private:
	GLuint programID;
	std::unordered_map<std::string, GLint> uniformLocationCache;

	// Helpers
	std::string readFile(const std::string& filepath);
	GLuint compileShader(GLenum type, const std::string& source);
	GLint getUniformLocation(const std::string& name);

public:
	Shader(): programID(0) {}
	~Shader();
	
	// Main
	void loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
	void loadFromStrings(const std::string& vertexSourse, const std::string& fragmentSource);
	void use() const;
	void unuse() const;

	// For uniform fields
	void setUniform1i(const std::string& name, int value);
	void setUniform1f(const std::string& name, float value);
	void setUniform2f(const std::string& name, float v0, float v1);
	void setUniform3f(const std::string& name, float v0, float v1, float v2);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniform1fv(const std::string& name, int count, const float* value);
	void setUniform3fv(const std::string& name, int count, const float* value);
	void setUniform4fv(const std::string& name, int count, const float* value);
	void setUniformMatrix4fv(const std::string& name, const float* matrix);

	bool checkCompileErrors(GLuint shader, const std::string& type);
};

#endif // !SHADER_H
