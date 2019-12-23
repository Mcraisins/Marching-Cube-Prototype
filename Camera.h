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

enum btype_enum {
	Diamond = 0,
	Cube
};

class Camera
{
public:
	static Camera * get();

	void rotate(glm::vec3);
	void reset();
	void controls();

	glm::vec3 getfacing();
	glm::vec3 getup();
	glm::vec3 camerapos;

	btype_enum btypeval = Diamond;
	int brushSize = 2;
	float brushDist;
	bool plstat = false;
	glm::vec3 plpos;

private:
	glm::vec3 camerafront, cameraup, cameraright;

	double currentTime;

	Camera();
	~Camera();
};

