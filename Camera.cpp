#include "Camera.h"
#include "Gui.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

static Camera * camera;

Camera * Camera::get()
{
	if(camera == NULL)
		camera = new Camera();
	return camera;
}

Camera::Camera()
{
	currentTime = glfwGetTime();
	camerapos = glm::vec3(0.5f, 0.5f, -3.0f);
	camerafront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraup = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraright = glm::cross(camerafront, cameraup);

	float brushSize = brushDist = 1;
}


Camera::~Camera()
{
	delete Camera::get();
}

void Camera::reset()
{
	camerapos = glm::vec3(
		(ModelLoader::get()->maxX) / 2, 
		(ModelLoader::get()->maxY) / 2,
		((ModelLoader::get()->maxZ) / 2)- 3);
	camerafront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraup = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraright = glm::cross(camerafront, cameraup);
}

void Camera::rotate(glm::vec3 rotateby)
{
	if(glm::length(rotateby) != 0)
	{
		glm::mat4 rotationmat = glm::rotate(glm::mat4(1.0f), (float)glm::radians(glm::length(rotateby)), glm::normalize(rotateby));
		camerafront = glm::normalize(glm::mat3(rotationmat) * camerafront);
		cameraup = glm::normalize(glm::mat3(rotationmat) * cameraup);
		cameraright = glm::normalize(glm::cross(camerafront, cameraup));
	}
}

glm::vec3 Camera::getfacing()
{ return camerafront; }

glm::vec3 Camera::getup()
{ return cameraup; }

void Camera::controls()
{
	double lastTime = currentTime;
	currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	deltaTime *= 2;

	GLFWwindow* window = Gui::get()->getWindow();

	// Strafing
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camerapos += camerafront * deltaTime;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camerapos -= camerafront * deltaTime;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camerapos += cameraright * deltaTime;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camerapos -= cameraright * deltaTime;
	}
	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camerapos += cameraup * deltaTime;
	}
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camerapos -= cameraup * deltaTime;
	}

	//Rotation
	if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		rotate(glm::normalize(cameraright) * deltaTime * Gui::get()->getrotateval());
	}
	if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		rotate(glm::normalize(cameraright) * -deltaTime * Gui::get()->getrotateval());
	}
	if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		rotate(glm::normalize(cameraup) * -deltaTime * Gui::get()->getrotateval());
	}
	if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		rotate(glm::normalize(cameraup) * deltaTime * Gui::get()->getrotateval());
	}
	if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		rotate(glm::normalize(camerafront) * -deltaTime * Gui::get()->getrotateval());
	}
	if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		rotate(glm::normalize(camerafront) * deltaTime * Gui::get()->getrotateval());
	}

	//PLight
	if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		plpos = camerapos;
		plstat = true;
	}
	if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		plstat = false;
	}

	//Drawing
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		ModelLoader::get()->brush();
	}
	//Drawing
	if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		reset();
	}
}