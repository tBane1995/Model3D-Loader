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
#include "Materials.hpp"
#include "Model3D.hpp"

GLFWwindow* window;
double cur_pos_x, cur_pos_y;	// cursor position

void events() {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float dt = current_time - prev_time;

	// WASD
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::FORWARD, dt);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::BACKWARD, dt);
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::LEFT, dt);
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::RIGHT, dt);

	// DIRECTION KEYS
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::FORWARD, dt);

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::BACKWARD, dt);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::LEFT, dt);

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cam->processKeyboard(Camera_Movement::RIGHT, dt);
	
	double new_cur_pos_x, new_cur_pos_y;
	glfwGetCursorPos(window, &new_cur_pos_x, &new_cur_pos_y);

	if (float(new_cur_pos_x) != cur_pos_x || float(new_cur_pos_y) != cur_pos_y) {
		cam->processMouseMovement(new_cur_pos_x - cur_pos_x, cur_pos_y - new_cur_pos_y);
		cur_pos_x = new_cur_pos_x;
		cur_pos_y = new_cur_pos_y;

	}


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
	addTexture(L"tex\\black");
	addTexture(L"tex\\green");
	addTexture(L"tex\\brown");

	// load programs
	addProgram(L"normal program", vertex_shader_source, fragment_shader_source);
	addProgram(L"advanced program", vertex_shader_with_light_source, fragment_shader_with_light_source);

	// load materials
	addLibMaterials(L"mtl\\fir_tree.mtl");
	addLibMaterials(L"mtl\\oak_tree.mtl");
	addLibMaterials(L"mtl\\oak2_tree.mtl");

	// load test_model
	Model3D test_mdl_1;
	test_mdl_1.load(L"mdl\\fir_tree.obj");
	test_mdl_1.setPosition(-3, 0, 0);

	Model3D test_mdl_2;
	test_mdl_2.load(L"mdl\\oak_tree.obj");
	test_mdl_2.setPosition(3, 0, 0);

	// cursor position
	glfwGetCursorPos(window, &cur_pos_x, &cur_pos_y);

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

		test_mdl_1.draw();
		test_mdl_2.draw();

		// render - submit
		glfwSwapBuffers(window);
		glfwPollEvents();

	}




	return 0;
}