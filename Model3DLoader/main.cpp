#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "WideUTF8Converter.hpp"

#include "Time.hpp"
#include "Camera.hpp"
#include "Transform.hpp"

#define STB_IMAGE_IMPLEMENTATION	// for work of stb_image.h
#include "stb_image.h"

#include "Textures.hpp"
#include "Shaders.hpp"
#include "Programs.hpp"
#include "Model3D.hpp"

GLFWwindow* window;

void events() {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float dt = current_time - prev_time;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::FORWARD, dt);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::BACKWARD, dt);
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::LEFT, dt);
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::RIGHT, dt);

	//std::cout << "camera position: (" << cam->position.x << ", " << cam->position.y << ", " << cam->position.z << ")\n";
}

int main() {

	

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


	const GLubyte* version = glGetString(GL_VERSION);
	if (version != nullptr) {
		std::cout << "OpenGL version: " << version << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH);

	cam = new Camera();
	cam->setPosition(glm::vec3(0, 5, 15));

	// load textures
	add_texture(L"tex\\black");
	//add_texture(L"tex\\green");
	//add_texture(L"tex\\brown");

	// load programs
	addProgram(vertex_shader_with_light_source, fragment_shader_with_light_source);

	// load test_model
	Model3D test_mdl;
	test_mdl.load(L"mdl\\fir_tree.obj");

	// timers start
	current_time = glfwGetTime();
	prev_time = current_time;

	// main program loop
	while (!glfwWindowShouldClose(window))
	{
		prev_time = current_time;
		current_time = glfwGetTime();


		// events
		events();

		// render - clear
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		test_mdl.draw();

		// render - submit
		glfwSwapBuffers(window);
		glfwPollEvents();

	}




	return 0;
}