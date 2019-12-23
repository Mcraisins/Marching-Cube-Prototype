#pragma once

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "nanogui/nanogui.h"
#include "Shader.h"

enum rtype_enum {
	Point = 0,
	Line,
	Triangle
};

class ModelLoader
{
public:
	static ModelLoader * get();
	void draw();
	void marchCubes();
	void loadCanvas();
	void brush();

	int currentSize = 0;

	float maxX = 1, maxY = 1, maxZ = 1, minValue = 0.5;
	int dataDense = 10;
	rtype_enum rtypeval = Triangle;
	nanogui::Color colval; 
	Shader * shader;

private:
	void cubeSurface(std::vector<glm::vec4>);
	float calcVal(glm::vec3);
	glm::vec4 LinearInterp(glm::vec4, glm::vec4, float);

	std::vector<float> canvasData;
	int canvasx, canvasy, canvasz;

	GLuint positionbuffer, normalbuffer, tangentbuffer, bitangentbuffer, elementbuffer;
	
	GLuint VertexArrayID;

	glm::vec3 oldCubeDim;
	float oldminValue;

	ModelLoader();
	~ModelLoader();
};

