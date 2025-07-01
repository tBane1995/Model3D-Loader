#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fbxsdk.h>

#include "WideUTF8Converter.hpp"

#include "Window.hpp"
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
#include "AnimatedModel3D.hpp"


bool cur_press;
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
	


	if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {

		if (cur_press == false) {
			glfwGetCursorPos(window, &cur_pos_x, &cur_pos_y);
			cur_press = true;
		}

		double new_cur_pos_x, new_cur_pos_y;
		glfwGetCursorPos(window, &new_cur_pos_x, &new_cur_pos_y);

		if (float(new_cur_pos_x) != cur_pos_x || float(new_cur_pos_y) != cur_pos_y) {
			cam->processMouseMovement(new_cur_pos_x - cur_pos_x, cur_pos_y - new_cur_pos_y);
			cur_pos_x = new_cur_pos_x;
			cur_pos_y = new_cur_pos_y;

		}
	}else if(glfwGetMouseButton(window, 0) == GLFW_RELEASE) {

		cur_press = false;
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
	glfwSwapInterval(0); // off V-Sync then FPS > 75

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
	addTexture(L"tex\\gray");
	addTexture(L"tex\\grass");
	addTexture(L"tex\\KnightTexture");
	

	
	// load programs
	addProgram(L"normal program", vertex_shader_source, fragment_shader_source);
	addProgram(L"advanced program", vertex_shader_with_light_source, fragment_shader_with_light_source);
	

	
	// load materials
	addLibMaterials(L"mtl\\fir_tree.mtl");
	addLibMaterials(L"mtl\\oak_tree.mtl");
	addLibMaterials(L"mtl\\Knight.mtl");
	addLibMaterials(L"mtl\\tree.mtl");
	addLibMaterials(L"mtl\\rock.mtl");
	addLibMaterials(L"mtl\\grass.mtl");
	

	
	// load test_model
	Model3D plane;
	plane.loadObj(L"mdl\\grass.obj");
	plane.setPosition(0, 0, 0);

	Model3D left_tree;
	left_tree.loadObj(L"mdl\\fir_tree.obj");
	left_tree.setPosition(-5, 0, 0);

	Model3D right_tree;
	right_tree.loadObj(L"mdl\\tree.obj");
	right_tree.setPosition(5, 0, 0);

	//Model3D knight;
	//knight.loadObj(L"mdl\\Knight.obj");
	//knight.setScale(2, 2, 2);
	//knight.setPosition(0, 0, 5);

	Model3D rock;
	rock.loadObj(L"mdl\\rock.obj");
	rock.setScale(2, 2, 2);
	rock.setPosition(0, 0, -5);

	

	AnimatedModel3D knightFBX;
	knightFBX.loadFBX();
	knightFBX.setPosition(0, 0, 5);

	// cursor position
	cur_press = false;
	glfwGetCursorPos(window, &cur_pos_x, &cur_pos_y);

	// timers start
	current_time = glfwGetTime();
	prev_time = current_time;
	
	float fps_timer = glfwGetTime();
	float FPS = 0;
	// main program loop
	while (!glfwWindowShouldClose(window))
	{
		prev_time = current_time;
		current_time = glfwGetTime();

		FPS += 1;
		if (current_time - fps_timer >= 1.0) {
			//std::cout << "FPS: " << FPS << "\n";
			glfwSetWindowTitle(window, std::to_string(FPS).c_str());
			FPS = 0;
			fps_timer = current_time;
		}

		// events
		events();

		// render - clear
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		plane.draw();
		left_tree.draw();
		right_tree.draw();
		knightFBX.draw();
		rock.draw();
		

		// render - submit
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	return 0;
}