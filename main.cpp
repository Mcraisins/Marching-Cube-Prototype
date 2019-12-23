#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

// Other includes
#include "Shader.h"
#include "Gui.h"
#include "ModelLoader.h"

int main()
{
	Gui * gui = Gui::get();
	ModelLoader * modelload = ModelLoader::get();
	Camera * camera = Camera::get();

	while(!glfwWindowShouldClose(gui->getWindow()))
	{
		glfwPollEvents();

		camera->controls();
		modelload->draw();
		gui->draw();

		glfwSwapBuffers(gui->getWindow());
	}
	return 0;
}