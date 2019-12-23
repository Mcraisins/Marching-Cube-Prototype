#include "Gui.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

using namespace nanogui;

static Gui * gui;

Gui * Gui::get()
{
	if(gui == NULL)
		gui = new Gui();
	return gui;
}

Gui::Gui()
{
	setupOpenGL();

	ngscreen = new Screen();
	ngscreen->initialize(window, true);

	bool enabled = true;
	FormHelper *gui = new FormHelper(ngscreen);
	ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(0, 0), "Nanogui control bar");

	gui->addGroup("Drawing");
	gui->addVariable("Brush Distance", Camera::get()->brushDist)->setSpinnable(true);
	gui->addVariable("Brush Size", Camera::get()->brushSize)->setSpinnable(true);
	gui->addVariable("Brush Type", Camera::get()->btypeval, enabled)->setItems({ "Diamond", "Cube" });
	gui->addVariable("Rotate Value", rotateval)->setSpinnable(true);
	gui->addButton("Reset Camera", []() { Camera::get()->reset(); });

	gui->addGroup("Cubes");
	Widget *intensityslider = new Widget(nanoguiWindow);
	intensityslider->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));
	Slider *slider = new Slider(intensityslider);
	slider->setValue(1.0f);
	slider->setFixedWidth(80);

	TextBox *textBox = new TextBox(intensityslider);
	textBox->setFixedSize(Vector2i(60, 25));
	textBox->setValue("0.5000");

	slider->setCallback([textBox](float value) {
		ModelLoader::get()->minValue = value;
		if(value > 0.99)
		{
			textBox->setValue("1.0000");
			ModelLoader::get()->minValue = 1.0f;
		}
		textBox->setValue(std::to_string(value));
	});

	gui->addWidget("Intensity Threshold", intensityslider);
	gui->addVariable("Render Type", ModelLoader::get()->rtypeval, enabled)->setItems({ "Point", "Line", "Triangle" });

	gui->addGroup("Canvas");
	gui->addVariable("Max X", ModelLoader::get()->maxX)->setSpinnable(true);
	gui->addVariable("Max Y", ModelLoader::get()->maxY)->setSpinnable(true);
	gui->addVariable("Max Z", ModelLoader::get()->maxZ)->setSpinnable(true);
	gui->addVariable("Data Density", ModelLoader::get()->dataDense);
	gui->addButton("Clear & Reload Canvas", []() { ModelLoader::get()->loadCanvas(); });

	FormHelper *gui2 = new FormHelper(ngscreen);
	ref<Window> nanoguiWindow2 = gui2->addWindow(Eigen::Vector2i(300, 0), "Nanogui control bar 2");
	gui2->addGroup("Lighting");
	gui->addVariable("Object Color", ModelLoader::get()->colval);
	gui2->addVariable("Object Shininess", shiny);
	gui2->addVariable("Camera Light Ambient Color", dlambient);
	gui2->addVariable("Camera Light Diffuse Color", dldiffuse);
	gui2->addVariable("Point Light Ambient Color", plambient);
	gui2->addVariable("Point Light Diffuse Color", pldiffuse);
	gui2->addVariable("Point Light Specular Color", plspec);

	ngscreen->setVisible(true);
	ngscreen->performLayout();

	glfwSetCursorPosCallback(window,
		[](GLFWwindow *, double x, double y) {
		Gui::get()->ngscreen->cursorPosCallbackEvent(x, y);
	}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow *, int button, int action, int modifiers) {
		Gui::get()->ngscreen->mouseButtonCallbackEvent(button, action, modifiers);
	}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow *, int key, int scancode, int action, int mods) {
		Gui::get()->ngscreen->keyCallbackEvent(key, scancode, action, mods);
	}
	);

	glfwSetCharCallback(window,
		[](GLFWwindow *, unsigned int codepoint) {
		Gui::get()->ngscreen->charCallbackEvent(codepoint);
	}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow *, int count, const char **filenames) {
		Gui::get()->ngscreen->dropCallbackEvent(count, filenames);
	}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow *, double x, double y) {
		Gui::get()->ngscreen->scrollCallbackEvent(x, y);
	}
	);


	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow *, int width, int height) {
		Gui::get()->ngscreen->resizeCallbackEvent(width, height);
	}
	);
}

void Gui::setupOpenGL()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(width, height, "Assignment 4", nullptr, nullptr);
	if(window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);

#if defined(NANOGUI_GLAD)
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Could not initialize GLAD!");
	glGetError();
#endif

	glewExperimental = GL_TRUE;
	glewInit();

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);
	glfwSwapBuffers(window);

	
}

Gui::~Gui()
{
	glfwTerminate();
}

void Gui::draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	ngscreen->drawWidgets();
}

GLFWwindow * Gui::getWindow()
{
	return window;
}


float Gui::getrotateval()
{
	return rotateval;
}

Light Gui::getDLight()
{
	Light dl = { dlambient, dldiffuse, dlspec };
	return dl;
}

Light Gui::getPLight()
{
	Light pl = { plambient, pldiffuse, plspec };
	return pl;
}