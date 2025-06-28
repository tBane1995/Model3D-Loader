#ifndef Programs_hpp
#define Programs_hpp

class Program {
public:
	unsigned int vertex_shader;
	unsigned int fragment_shader;
	unsigned int shader_program;

	Program(const char* vertex_shader_source, const char* fragment_shader_source) {
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

void addProgram(const char* vertex_shader_source, const char* fragment_shader_source) {
	programs.push_back(new Program(vertex_shader_source, fragment_shader_source));
}

#endif