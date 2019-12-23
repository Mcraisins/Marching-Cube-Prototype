#include "ModelLoader.h"
#include "Camera.h"
#include "Gui.h"
#include "CubeTables.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <math.h>

static ModelLoader * modelLoader;

ModelLoader * ModelLoader::get()
{
	if(modelLoader == NULL)
		modelLoader = new ModelLoader();
	return modelLoader;
}

ModelLoader::ModelLoader()
{
	colval = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	shader = new Shader("basic.vert", "basic.frag");

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	loadCanvas();
}

ModelLoader::~ModelLoader()
{
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteBuffers(1, &positionbuffer);
	glDeleteBuffers(1, &elementbuffer);
	delete modelLoader;
}

void ModelLoader::loadCanvas()
{
	if((maxX) < 0 || (maxY) < 0 || (maxZ) < 0)
		return;
	canvasx = (int)((maxX)*dataDense);
	canvasy = (int)((maxY)*dataDense);
	canvasz = (int)((maxZ)*dataDense);

	canvasData.clear();  canvasData.resize(canvasx*canvasy*canvasz);
	marchCubes();
}

void ModelLoader::brush()
{
	glm::vec3 vec = Camera::get()->camerapos + (glm::normalize(Camera::get()->getfacing())*Camera::get()->brushDist);
	glm::vec3 incan = glm::vec3(vec);

	int xoffset = (int)(vec.x*dataDense);
	int yoffset = (int)(vec.y*dataDense);
	int zoffset = (int)(vec.z*dataDense);

	int brushSize = Camera::get()->brushSize;

	if(Camera::get()->btypeval == Diamond)
	for(int i = 1; i < (brushSize * 2); i++)
		for(int j = abs(brushSize-i) + 1; j < (brushSize * 2) - abs(brushSize-i); j++)
			for(int k = abs(brushSize-i) + abs(brushSize - j) + 1; k < (brushSize * 2) - abs(brushSize-i)-abs(brushSize-j); k++)
			{
				int nx = (xoffset + i - brushSize), ny = (yoffset + j - brushSize), nz = (zoffset + k - brushSize);

				if(nx >= canvasx || ny >= canvasy || nz >= canvasz) continue;
				if(nx < 0 || ny < 0 || nz < 0) continue;

				float diff = brushSize - abs(brushSize - i) - abs(brushSize - j) - abs(brushSize - k);
				if(canvasData[nx + (canvasx * ny) + (canvasx*canvasy*nz)] < 1.0f/diff) 
					canvasData[nx + (canvasx * ny) + (canvasx*canvasy*nz)] = 1.0f/diff;
			}
	else if(Camera::get()->btypeval == Cube)
		for(int i = 1; i < (brushSize * 2); i++)
			for(int j = 1; j < (brushSize * 2); j++)
				for(int k = 1; k < (brushSize * 2); k++)
				{
					int nx = (xoffset + i - brushSize), ny = (yoffset + j - brushSize), nz = (zoffset + k - brushSize);

					if(nx >= canvasx || ny >= canvasy || nz >= canvasz) continue;
					if(nx < 0 || ny < 0 || nz < 0) continue;

					float diff = brushSize*2 - std::max(std::max(std::max(i,j),k), abs(brushSize - std::min(std::min(i,j),k)));
					if(canvasData[nx + (canvasx * ny) + (canvasx*canvasy*nz)] < 1.0f / diff)
						canvasData[nx + (canvasx * ny) + (canvasx*canvasy*nz)] = 1.0f / diff;
				}
	marchCubes();
}

void ModelLoader::draw()
{
	glm::vec3 cubeDim(dataDense, dataDense, dataDense);
	if(oldCubeDim != cubeDim || oldminValue != minValue)
	{
		oldCubeDim = cubeDim;
		oldminValue = minValue;
		marchCubes();
	}

	glBindVertexArray(VertexArrayID);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int width, height;
	glfwGetFramebufferSize(Gui::get()->getWindow(), &width, &height);
	glViewport(0, 0, width, height);

	shader->use();
	shader->setUniform("usercolor", colval.x(), colval.y(), colval.z());
	shader->setUniform("view", glm::lookAt(Camera::get()->camerapos, Camera::get()->camerapos + Camera::get()->getfacing(), Camera::get()->getup()));
	shader->setUniform("projection", glm::perspective(glm::radians(45.0f), (float)width / (float)height,
		(float)0.01, (float)20));
	shader->setUniform("shininess", Gui::get()->shiny);

	shader->setUniform("viewpos", Camera::get()->camerapos.x, Camera::get()->camerapos.y, Camera::get()->camerapos.z);
	shader->setUniform("dirLight.direction", Camera::get()->getfacing().x, Camera::get()->getfacing().y, Camera::get()->getfacing().z);
	shader->setUniform("dirLight.on", true);

	Light dl = Gui::get()->getDLight();
	shader->setUniform("dirLight.ambient", dl.ambient.x(), dl.ambient.y(), dl.ambient.z());
	shader->setUniform("dirLight.diffuse", dl.diffuse.x(), dl.diffuse.y(), dl.diffuse.z());
	shader->setUniform("dirLight.specular", dl.spec.x(), dl.spec.y(), dl.spec.z());

	Light pl = Gui::get()->getPLight();
	shader->setUniform("pointLight.position", Camera::get()->plpos.x, Camera::get()->plpos.y, Camera::get()->plpos.z);
	shader->setUniform("pointLight.on", Camera::get()->plstat);
	shader->setUniform("pointLight.ambient", pl.ambient.x(), pl.ambient.y(), pl.ambient.z());
	shader->setUniform("pointLight.diffuse", pl.diffuse.x(), pl.diffuse.y(), pl.diffuse.z());
	shader->setUniform("pointLight.specular", pl.spec.x(), pl.spec.y(), pl.spec.z());
	shader->setUniform("pointLight.constant", 1.0f);
	shader->setUniform("pointLight.linear", 0.0);
	shader->setUniform("pointLight.quadratic", 0.0);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, positionbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	switch(rtypeval)
	{
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2: default:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glDrawElements(GL_TRIANGLES, currentSize, GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void ModelLoader::cubeSurface(std::vector<glm::vec4> points)
{
	//this should be enough space, if not change 3 to 4
	std::vector<glm::vec3> positions, normals, tangents, bitangents;
	std::vector<unsigned int> indices;
	currentSize = int(0);

	int YtimeZ = (dataDense + 1)*(dataDense + 1);	//for little extra speed
	int ni, nj;

	//go through all the points
	for(int i = 0; i < dataDense; i++) {		//x axis
		ni = i * YtimeZ;
		for(int j = 0; j < dataDense; j++) {	//y axis
			nj = j * (dataDense + 1);
			for(int k = 0; k < dataDense; k++)	//z axis
			{
				//initialize vertices
				std::vector<glm::vec4> verts(8);
				int ind = ni + nj + k;
				verts[0] = points[ind];
				verts[1] = points[ind + YtimeZ];
				verts[2] = points[ind + YtimeZ + 1];
				verts[3] = points[ind + 1];
				verts[4] = points[ind + (dataDense + 1)];
				verts[5] = points[ind + YtimeZ + (dataDense + 1)];
				verts[6] = points[ind + YtimeZ + (dataDense + 1) + 1];
				verts[7] = points[ind + (dataDense + 1) + 1];

				//get the index
				int cubeIndex = int(0);
				for(int n = 0; n < 8; n++)
					if(verts[n].w <= minValue) cubeIndex |= (1 << n);

				//check if its completely inside or outside
				if(!edgeTable[cubeIndex]) continue;

				//get linearly interpolated vertices on edges and save into the array
				std::vector<glm::vec4> intVerts(12);
				if(edgeTable[cubeIndex] & 1) intVerts[0] = LinearInterp(verts[0], verts[1], minValue);
				if(edgeTable[cubeIndex] & 2) intVerts[1] = LinearInterp(verts[1], verts[2], minValue);
				if(edgeTable[cubeIndex] & 4) intVerts[2] = LinearInterp(verts[2], verts[3], minValue);
				if(edgeTable[cubeIndex] & 8) intVerts[3] = LinearInterp(verts[3], verts[0], minValue);
				if(edgeTable[cubeIndex] & 16) intVerts[4] = LinearInterp(verts[4], verts[5], minValue);
				if(edgeTable[cubeIndex] & 32) intVerts[5] = LinearInterp(verts[5], verts[6], minValue);
				if(edgeTable[cubeIndex] & 64) intVerts[6] = LinearInterp(verts[6], verts[7], minValue);
				if(edgeTable[cubeIndex] & 128) intVerts[7] = LinearInterp(verts[7], verts[4], minValue);
				if(edgeTable[cubeIndex] & 256) intVerts[8] = LinearInterp(verts[0], verts[4], minValue);
				if(edgeTable[cubeIndex] & 512) intVerts[9] = LinearInterp(verts[1], verts[5], minValue);
				if(edgeTable[cubeIndex] & 1024) intVerts[10] = LinearInterp(verts[2], verts[6], minValue);
				if(edgeTable[cubeIndex] & 2048) intVerts[11] = LinearInterp(verts[3], verts[7], minValue);

				//now build the triangles using triTable
				for(int n = 0; triTable[cubeIndex][n] != -1; n += 3) {
					glm::vec3 pos1 = intVerts[triTable[cubeIndex][n + 2]];
					glm::vec3 pos2 = intVerts[triTable[cubeIndex][n + 1]];
					glm::vec3 pos3 = intVerts[triTable[cubeIndex][n + 0]];

					glm::vec3 U = pos2 - pos1, V = pos3 - pos1;
					
					glm::vec3 normal = glm::normalize(glm::cross(U, V));
					glm::vec3 tangent = glm::normalize(pos2 - pos1);
					glm::vec3 bitangent = glm::normalize(glm::cross(tangent, normal));

					positions.push_back(pos1); positions.push_back(pos2); positions.push_back(pos3);
					normals.push_back(normal); normals.push_back(normal); normals.push_back(normal);
					tangents.push_back(tangent); tangents.push_back(tangent); tangents.push_back(tangent);
					bitangents.push_back(bitangent); bitangents.push_back(bitangent); bitangents.push_back(bitangent);

					indices.push_back(currentSize++); indices.push_back(currentSize++); indices.push_back(currentSize++);
				}

			}	//END OF Z FOR LOOP
		}	//END OF Y FOR LOOP
	}	//END OF X FOR LOOP

	glDeleteBuffers(1, &positionbuffer);
	glGenBuffers(1, &positionbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionbuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);

	glDeleteBuffers(1, &normalbuffer);
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glDeleteBuffers(1, &tangentbuffer);
	glGenBuffers(1, &tangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

	glDeleteBuffers(1, &bitangentbuffer);
	glGenBuffers(1, &bitangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);

	glDeleteBuffers(1, &elementbuffer);
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void ModelLoader::marchCubes()
{
	std::vector<glm::vec4> mcDataPoints((dataDense + 1)*(dataDense + 1)*(dataDense + 1));
	glm::vec3 stepSize((maxX) / dataDense, (maxY) / dataDense, (maxZ) / dataDense);

	int YtimesZ = (dataDense + 1)*(dataDense + 1);		//for little extra speed
	for(int i = 0; i < dataDense + 1; i++) {
		int ni = i * YtimesZ;						//for little extra speed
		float vertX = i * stepSize.x;
		for(int j = 0; j < dataDense + 1; j++) {
			int nj = j * (dataDense + 1);				//for little extra speed
			float vertY = j * stepSize.y;
			for(int k = 0; k < dataDense + 1; k++) {
				glm::vec3 temp(vertX, vertY, k * stepSize.z);
				glm::vec4 vert(vertX, vertY, k * stepSize.z, calcVal(temp));
				mcDataPoints[ni + nj + k] = vert;
			}
		}
	}
	//then run Marching Cubes (version 1) on the data
	return cubeSurface(mcDataPoints);
}

glm::vec4 ModelLoader::LinearInterp(glm::vec4 p1, glm::vec4 p2, float value)
{
	
	glm::vec4 p;
	if(p1.w != p2.w)
		p = (glm::vec4)p1 + ((glm::vec4)p2 - (glm::vec4)p1) / (p2.w - p1.w)*(value - p1.w);
	else
		p = (glm::vec4)p1;
	return p;
}

using namespace std;


float ModelLoader::calcVal(glm::vec3 vec)
{
	int xoffset = (int)(vec.x*dataDense);
	int yoffset = (int)(vec.y*dataDense);
	int zoffset = (int)((vec.z)*dataDense);

	if(xoffset >= canvasx || yoffset >= canvasy || zoffset >= canvasz)
		return -1;
	if(xoffset < 0 || yoffset < 0 || zoffset < 0)
		return -1;

	float x = sin(vec.x*6)* sin(vec.y*3) * sin(vec.z*4);
	return canvasData[xoffset + (canvasx*yoffset) + (canvasx*canvasy*zoffset)];
}