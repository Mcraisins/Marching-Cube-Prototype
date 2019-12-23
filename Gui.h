#pragma once

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "nanogui/nanogui.h"
#include "Camera.h"
#include "ModelLoader.h"

const GLuint width = 1024, height = 768;

typedef struct {
	nanogui::Color ambient;
	nanogui::Color diffuse;
	nanogui::Color spec;
} Light;

class Gui 
{
public:
	static Gui * get();

	void setupOpenGL();
	GLFWwindow * getWindow();
	float getrotateval();
	Light getPLight();
	Light getDLight();
	void draw();

	float shiny = 32.0f;

private:
	Gui();
	~Gui();

	GLFWwindow * window;
	nanogui::Screen * ngscreen;
	float rotateval = 20.0f;

	nanogui::Color dlambient = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	nanogui::Color dldiffuse = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	nanogui::Color dlspec = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

	nanogui::Color plambient = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	nanogui::Color pldiffuse = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	nanogui::Color plspec = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
};

