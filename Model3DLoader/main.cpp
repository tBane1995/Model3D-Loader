#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;

std::string ConvertWideToUtf8(std::wstring wide) {
	return std::string(wide.begin(), wide.end());
}

class Model3D {
public:

	std::vector < float > v;	// vertices
	std::vector < float > vt;	// texture uv
	std::vector < float > vn;	// normals

	std::vector < std::vector < unsigned int > > meshes;	// ?? // std::vector < face > meshes
	//std::vector < Texture* > textures;

	unsigned int VAO;


	Model3D() {

	}

	~Model3D() {

		v.clear();	// vertices
		vt.clear();	// texture uv
		vn.clear();	// normals

		meshes.clear();

	}

	void load(std::wstring path) {

		std::wifstream file(path);
		if (!file.is_open()) {
			std::cout << "can't open file : " << ConvertWideToUtf8(path) << "\n";
			return;
		}

		std::wstring line;
		while (std::getline(file, line)) {

			std::cout << ConvertWideToUtf8(line) << "\n";

			if (line.empty())
				continue;

			std::wistringstream wstream(line);

			std::wstring prefix;
			wstream >> prefix;

			if (prefix == L"v") {
				// vertice
				float x, y, z;
				wstream >> x >> y >> z;

				v.push_back(x);
				v.push_back(y);
				v.push_back(z);
			}
			else if (prefix == L"vt") {
				// texture uv
				float u, v;
				wstream >> u >> v;

				vt.push_back(u);
				vt.push_back(v);
			}
			else if (prefix == L"vn") {
				// normal
				float nx, ny, nz;
				wstream >> nx >> ny >> nz;

				vn.push_back(nx);
				vn.push_back(ny);
				vn.push_back(nz);
			}
			else if (prefix == L"f") {
				// face
				// TO-DO
			}
		}


	}

	void draw() {

	}
};

void events() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

int main() {
	Model3D test_mdl;
	test_mdl.load(L"mdl\\fir_tree.obj");

	// initialize glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "Model3D Loader", NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// set the context
	glfwMakeContextCurrent(window);

	// initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	const GLubyte* version = glGetString(GL_VERSION);
	if (version != nullptr) {
		std::cout << "OpenGL version: " << version << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH);

	// main program loop
	while (!glfwWindowShouldClose(window))
	{

		// events
		events();

		// render - clear
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//
		// .. rendering objects
		//

		// render - submit
		glfwSwapBuffers(window);
		glfwPollEvents();

	}




	return 0;
}