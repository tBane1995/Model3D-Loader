#ifndef Programs_hpp
#define Programs_hpp

class Program {
public:
	std::wstring name;
	unsigned int vertex_shader;
	unsigned int fragment_shader;
	unsigned int shader_program;

	Program(std::wstring name, const char* vertex_shader_source, const char* fragment_shader_source) {

		this->name = name;

		// vertex shader
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
		glCompileShader(vertex_shader);

		// fragment shader
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
		glCompileShader(fragment_shader);

		// program
		shader_program = glCreateProgram();
		glAttachShader(shader_program, vertex_shader);
		glAttachShader(shader_program, fragment_shader);
		glLinkProgram(shader_program);
	}

	~Program() {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(shader_program);

	}
};

std::vector < Program* > programs;

void addProgram(std::wstring name, const char* vertex_shader_source, const char* fragment_shader_source) {
	programs.push_back(new Program(name, vertex_shader_source, fragment_shader_source));
	//std::wcout << L"loaded program: " << name << "\n";
}

Program* getProgram(std::wstring name) {
	for (auto& p : programs)
		if (p->name == name)
			return p;

	return nullptr;
}

#endif