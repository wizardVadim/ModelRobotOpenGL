#include "Shader.h"

Shader::~Shader() {
	if (programID != 0) {
		glDeleteProgram(programID);
	}
}

std::string Shader::readFile(const std::string& filepath) {

	std::ifstream file;
	std::stringstream stream;

	file.open(filepath);
	if (!file.is_open()) {
		std::cerr << "ERROR: Couldn't open file: " << filepath << std::endl;
		return "";
	}

	stream << file.rdbuf();
	file.close();

	return stream.str();

}

GLuint Shader::compileShader(GLenum type, const std::string& source) {

	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	// Check compile errors
	std::string shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
	if (!checkCompileErrors(shader, shaderType)) {
		glDeleteShader(shader);
		return 0;
	}

	return shader;

}

bool Shader::checkCompileErrors(GLuint shader, const std::string& type) {
	
	GLint success;
	GLchar infoLog[1024];

	if (type != "PROGRAM") {
		
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			// ÄËß ËÎÃÀ ØÅÉÄÅÐÀ - 4 àðãóìåíòà
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			return false;
		}

	} else {

		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			// ÄËß ËÎÃÀ ÏÐÎÃÐÀÌÌÛ - 4 àðãóìåíòà
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			return false;
		}

	}

	return true;

}

void Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
	
	std::string vertexCode = readFile(vertexPath);
	std::string fragmentCode = readFile(fragmentPath);

	if (vertexCode.empty() || fragmentCode.empty()) {
		std::cerr << "ERROR: Failed to load shader files!" << std::endl;
		return;
	}

	loadFromStrings(vertexCode, fragmentCode);

}

void Shader::loadFromStrings(const std::string& vertexSource, const std::string& fragmentSource) {

	// Compile shaders
	GLuint vertexSh = compileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentSh = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

	if (vertexSh == 0 || fragmentSh == 0) {
		return;
	}

	// Create program
	programID = glCreateProgram();
	glAttachShader(programID, vertexSh);
	glAttachShader(programID, fragmentSh);
	glLinkProgram(programID);

	if (!checkCompileErrors(programID, "PROGRAM")) {
		programID = 0;
	}

	// Del shaders
	glDeleteShader(vertexSh);
	glDeleteShader(fragmentSh);

}

void Shader::use() const {

	glUseProgram(programID);

}

void Shader::unuse() const {

	glUseProgram(0);

}

GLint Shader::getUniformLocation(const std::string& name) {

	// Check cache
	auto it = uniformLocationCache.find(name);
	if (it != uniformLocationCache.end()) {
		return it->second;
	}

	GLint location = glGetUniformLocation(programID, name.c_str());
	if (location == -1) {
		std::cerr << "WARNING: Uniform '" << name << "' doesn't exist!" << std::endl;
	}

	uniformLocationCache[name] = location;
	return location;

}

void Shader::setUniform1i(const std::string& name, int value) {
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1f(const std::string& name, float value) {
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform2f(const std::string& name, float v0, float v1) {
	glUniform2f(getUniformLocation(name), v0, v1);
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2) {
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniform1fv(const std::string& name, int count, const float* value) {
	glUniform1fv(getUniformLocation(name), count, value);
}

void Shader::setUniform3fv(const std::string& name, int count, const float* value) {
	glUniform3fv(getUniformLocation(name), count, value);
}

void Shader::setUniform4fv(const std::string& name, int count, const float* value) {
	glUniform4fv(getUniformLocation(name), count, value);
}

void Shader::setUniformMatrix4fv(const std::string& name, const float* matrix) {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix);
}